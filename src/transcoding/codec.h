/*
 *  tvheadend - Codec Profiles
 *
 *  Copyright (C) 2016 Tvheadend
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef TVH_TRANSCODING_CODEC_H__
#define TVH_TRANSCODING_CODEC_H__

#include "build.h"

#if ENABLE_LIBAV

#include "tvheadend.h"
#include "idnode.h"
#include "streaming.h"
#include "libav.h"

#include <libavcodec/avcodec.h>


#define tvh_ssc_t streaming_start_component_t
#define tvh_sct_t streaming_component_type_t


struct tvh_codec_t;
typedef struct tvh_codec_t TVHCodec;

struct tvh_codec_profile_t;
typedef struct tvh_codec_profile_t TVHCodecProfile;


/* codec_profile_class ====================================================== */

typedef int (*codec_profile_setup_meth) (TVHCodecProfile *, tvh_ssc_t *);
typedef int (*codec_profile_is_copy_meth) (TVHCodecProfile *, tvh_ssc_t *);
typedef int (*codec_profile_open_meth) (TVHCodecProfile *, AVDictionary **);

typedef struct {
    idclass_t idclass;
    codec_profile_setup_meth setup;
    codec_profile_is_copy_meth is_copy;
    codec_profile_open_meth open;
} codec_profile_class_t;


/* TVHCodec ================================================================= */

typedef struct tvh_codec_t {
    const char *name;
    size_t size;
    const codec_profile_class_t *idclass;
    AVCodec *codec;
    const AVProfile *profiles;
    int (*profile_init)(TVHCodecProfile *, htsmsg_t *conf);
    SLIST_ENTRY(tvh_codec_t) link;
} TVHCodec;

SLIST_HEAD(TVHCodecs, tvh_codec_t);
extern struct TVHCodecs tvh_codecs;

const idclass_t *
tvh_codec_get_class(TVHCodec *self);

const char *
tvh_codec_get_name(TVHCodec *self);

const char *
tvh_codec_get_title(TVHCodec *self);


/* TVHCodecProfile ========================================================== */

extern const codec_profile_class_t codec_profile_class;

typedef struct tvh_codec_profile_t {
    idnode_t idnode;
    TVHCodec *codec;
    const char *name;
    const char *description;
    const char *codec_name;
    double bit_rate;
    double qscale;
    int profile;
    char *device; // for hardware acceleration
    LIST_ENTRY(tvh_codec_profile_t) link;
} TVHCodecProfile;

LIST_HEAD(TVHCodecProfiles, tvh_codec_profile_t);
extern struct TVHCodecProfiles tvh_codec_profiles;

int
tvh_codec_profile_create(htsmsg_t *conf, const char *uuid, int save);

const char *
tvh_codec_profile_get_status(TVHCodecProfile *self);

const char *
tvh_codec_profile_get_name(TVHCodecProfile *self);

const char *
tvh_codec_profile_get_title(TVHCodecProfile *self);

AVCodec *
tvh_codec_profile_get_avcodec(TVHCodecProfile *self);


/* transcode api */
int
tvh_codec_profile_is_copy(TVHCodecProfile *self, tvh_ssc_t *ssc); // XXX: not too sure...

int
tvh_codec_profile_open(TVHCodecProfile *self, AVDictionary **opts);


/* video */
int
tvh_codec_profile_video_init(TVHCodecProfile *_self, htsmsg_t *conf);

int
tvh_codec_profile_video_get_hwaccel(TVHCodecProfile *self);

const enum AVPixelFormat *
tvh_codec_profile_video_get_pix_fmts(TVHCodecProfile *self);


/* audio */
int
tvh_codec_profile_audio_init(TVHCodecProfile *_self, htsmsg_t *conf);

const enum AVSampleFormat *
tvh_codec_profile_audio_get_sample_fmts(TVHCodecProfile *self);

const int *
tvh_codec_profile_audio_get_sample_rates(TVHCodecProfile *self);

const uint64_t *
tvh_codec_profile_audio_get_channel_layouts(TVHCodecProfile *self);


/* module level ============================================================= */

TVHCodecProfile *
codec_find_profile(const char *name);

htsmsg_t *
codec_get_profiles_list(enum AVMediaType media_type);

void
codec_init(void);

void
codec_done(void);

#else

static inline void
codec_init(void)
{
}

static inline void
codec_done(void)
{
}

#endif

#endif // TVH_TRANSCODING_CODEC_H__
