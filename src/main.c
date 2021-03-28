#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <vorbis/vorbisfile.h>
#include <string.h>
#define WRITE_U32(buf, x)                               \
    *(buf) = (unsigned char)((x)&0xff);                 \
    *((buf) + 1) = (unsigned char)(((x) >> 8) & 0xff);  \
    *((buf) + 2) = (unsigned char)(((x) >> 16) & 0xff); \
    *((buf) + 3) = (unsigned char)(((x) >> 24) & 0xff);

#define WRITE_U16(buf, x)               \
    *(buf) = (unsigned char)((x)&0xff); \
    *((buf) + 1) = (unsigned char)(((x) >> 8) & 0xff);

static int quiet = 0;
static int bits = 16;
static int endian = 0;
static int sign = 1;
unsigned char headbuf[44]; /* The whole buffer */
/* Some of this based on ao/src/ao_wav.c */
int write_prelim_header(OggVorbis_File *vf, FILE *out, ogg_int64_t knownlength)
{
    unsigned int size = 0x7fffffff;
    int channels = ov_info(vf, 0)->channels;
    int samplerate = ov_info(vf, 0)->rate;
    int bytespersec = channels * samplerate * bits / 8;
    int align = channels * bits / 8;
    int samplesize = bits;

    if (knownlength && knownlength * bits / 8 * channels < size)
        size = (unsigned int)(knownlength * bits / 8 * channels + 44);

    memcpy(headbuf, "RIFF", 4);
    WRITE_U32(headbuf + 4, size - 8);
    memcpy(headbuf + 8, "WAVE", 4);
    memcpy(headbuf + 12, "fmt ", 4);
    WRITE_U32(headbuf + 16, 16);
    WRITE_U16(headbuf + 20, 1); /* format */
    WRITE_U16(headbuf + 22, channels);
    WRITE_U32(headbuf + 24, samplerate);
    WRITE_U32(headbuf + 28, bytespersec);
    WRITE_U16(headbuf + 32, align);
    WRITE_U16(headbuf + 34, samplesize);
    memcpy(headbuf + 36, "data", 4);
    WRITE_U32(headbuf + 40, size - 44);

    if (fwrite(headbuf, 1, 44, out) != 44)
    {
        fprintf(stderr, "ERROR: Failed to write Wave header: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}

int rewrite_header(FILE *out, unsigned int written)
{
    unsigned int length = written;

    length += 44;

    WRITE_U32(headbuf + 4, length - 8);
    WRITE_U32(headbuf + 40, length - 44);
    if (fseek(out, 0, SEEK_SET) != 0)
        return 1;

    if (fwrite(headbuf, 1, 44, out) != 44)
    {
        fprintf(stderr, "ERROR: Failed to write Wave header: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}
static void
permute_channels(char *in, char *out, int len, int channels, int bytespersample)
{
    int permute[6][6] = {{0}, {0, 1}, {0, 2, 1}, {0, 1, 2, 3}, {0, 1, 2, 3, 4}, {0, 2, 1, 5, 3, 4}};
    int i, j, k;
    int samples = len / channels / bytespersample;

    /* Can't handle, don't try */
    if (channels > 6)
        return;

    for (i = 0; i < samples; i++)
    {
        for (j = 0; j < channels; j++)
        {
            for (k = 0; k < bytespersample; k++)
            {
                out[i * bytespersample * channels +
                    bytespersample * permute[channels - 1][j] + k] =
                    in[i * bytespersample * channels + bytespersample * j + k];
            }
        }
    }
}

int decode(char *infile, char *outfile)
{
    FILE *in, *out;
    in = fopen(infile, "rb");
    if (!in)
    {
        fprintf(stderr, "ERROR: Failed to open input file: %s\n", strerror(errno));
        return 1;
    }
    out = fopen(outfile, "wb");
    if (!out)
    {
        fprintf(stderr, "ERROR: Failed to open output file: %s\n", strerror(errno));
        fclose(in);
        return 1;
    }

    OggVorbis_File vf;
    int bs = 0;
    char buf[8192], outbuf[8192];
    char *p_outbuf;
    int buflen = 8192;
    unsigned int written = 0;
    int ret;
    ogg_int64_t length = 0;
    ogg_int64_t done = 0;
    int size = 0;
    int seekable = 0;
    int percent = 0;
    int channels;
    int samplerate;

    if (ov_open_callbacks(in, &vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
    {
        fprintf(stderr, "ERROR: Failed to open input as Vorbis\n");
        fclose(in);
        fclose(out);
        return 1;
    }

    channels = ov_info(&vf, 0)->channels;
    samplerate = ov_info(&vf, 0)->rate;

    if (ov_seekable(&vf))
    {
        int link;
        int chainsallowed = 0;
        for (link = 0; link < ov_streams(&vf); link++)
        {
            if (ov_info(&vf, link)->channels == channels &&
                ov_info(&vf, link)->rate == samplerate)
            {
                chainsallowed = 1;
            }
        }

        seekable = 1;
        if (chainsallowed)
            length = ov_pcm_total(&vf, -1);
        else
            length = ov_pcm_total(&vf, 0);
        size = bits / 8 * channels;
        if (!quiet)
            fprintf(stderr, "Decoding \"%s\" to \"%s\"\n",
                    infile ? infile : "standard input",
                    outfile ? outfile : "standard output");
    }
    if (write_prelim_header(&vf, out, length))
    {
        ov_clear(&vf);
        return 1;
    }
    if (channels > 2)
    {
        /* Then permute! */
        permute_channels(buf, outbuf, ret, channels, bits / 8);
        p_outbuf = outbuf;
    }
    else
    {
        p_outbuf = buf;
    }

    while ((ret = ov_read(&vf, buf, buflen, endian, bits / 8, sign, &bs)) != 0)
    {
        if (bs != 0)
        {
            vorbis_info *vi = ov_info(&vf, -1);
            if (channels != vi->channels || samplerate != vi->rate)
            {
                fprintf(stderr, "Logical bitstreams with changing parameters are not supported\n");
                break;
            }
        }

        if (ret == OV_HOLE)
        {
            if (!quiet)
            {
                fprintf(stderr, "WARNING: hole in data (%d)\n", ret);
            }
            continue;
        }
        else if (ret < 0)
        {
            if (!quiet)
            {
                fprintf(stderr, "=== Vorbis library reported a stream error.\n");
            }
            ov_clear(&vf);
            fclose(out);
            return 1;
        }

        p_outbuf = buf;

        if (fwrite(p_outbuf, 1, ret, out) != ret)
        {
            fprintf(stderr, "Error writing to file: %s\n", strerror(errno));
            ov_clear(&vf);
            fclose(out);
            return 1;
        }

        written += ret;
        if (!quiet && seekable)
        {
            done += ret / size;
            if ((double)done / (double)length * 200. > (double)percent)
            {
                percent = (int)((double)done / (double)length * 200);
                fprintf(stderr, "\r\t[%5.1f%%]", (double)percent / 2.);
            }
        }
    }

    if (seekable && !quiet)
        fprintf(stderr, "\n");
    rewrite_header(out, written); /* We don't care if it fails, too late */

    ov_clear(&vf);
    fclose(out);
    return 0;
}
