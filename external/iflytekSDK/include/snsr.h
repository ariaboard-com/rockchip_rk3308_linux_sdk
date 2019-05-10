/*
 * Copyright (c) Sensory, Inc. 2000-2017.  All Rights Reserved.
 * http://sensory.com/
 *
 * You may not use these files except in compliance with the license.
 * A copy of the license is located the "LICENSE.txt" file accompanying
 * this source. This file is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *---------------------------------------------------------------------------
 * TrulyHandsfree SDK
 *---------------------------------------------------------------------------
 */
#ifndef SNSR_H
#define SNSR_H
#if defined(__cplusplus)
extern "C" {
#endif
#include <stdarg.h>
#ifndef _WIN32
# include <unistd.h>
#endif
#define SNSR_LICENSE_WARNING "license-exp-warn"
#define SNSR_LICENSE_EXPDATE "license-exp-date"
#define SNSR_LICENSE_EXPIRES "license-exp-message"
#define SNSR_LIBRARY_INFO "library-info"
#define SNSR_AUTO_FLUSH "auto-flush"
#define SNSR_TASK_TYPE "task-type"
#define SNSR_TASK_NAME "task-name"
#define SNSR_TASK_VERSION "task-version"
#define SNSR_PHRASESPOT "phrasespot"
#define SNSR_SPOT_DELAY "delay"
#define SNSR_LISTEN_WINDOW "listen-window"
#define SNSR_EMBEDDED_TARGET "dsp-target"
#define SNSR_EMBEDDED_HEADER_STREAM "dsp-header-stream"
#define SNSR_EMBEDDED_SEARCH_STREAM "dsp-search-stream"
#define SNSR_EMBEDDED_ACMODEL_STREAM "dsp-acmodel-stream"
#define SNSR_OPERATING_POINT "operating-point"
#define SNSR_OPERATING_POINT_LIST "operating-point-iterator"
#define SNSR_RES_AVAILABLE_POINT "available-point"
#define SNSR_SAMPLE_RATE "samples-per-second"
#define SNSR_AUDIO_STREAM_SIZE "audio-stream-size"
#define SNSR_AUDIO_STREAM_FIRST "audio-stream-first"
#define SNSR_AUDIO_STREAM_LAST "audio-stream-last"
#define SNSR_AUDIO_STREAM_FROM "audio-stream-from"
#define SNSR_AUDIO_STREAM_TO "audio-stream-to"
#define SNSR_SOURCE_AUDIO_PCM "audio-pcm"
#define SNSR_RESULT_EVENT "^result"
#define SNSR_SAMPLES_EVENT "^sample-count"
#define SNSR_RES_SAMPLES "sample-count"
#define SNSR_RES_BEGIN_MS "begin-ms"
#define SNSR_RES_BEGIN_SAMPLE "begin-sample"
#define SNSR_RES_END_MS "end-ms"
#define SNSR_RES_END_SAMPLE "end-sample"
#define SNSR_RES_SV_SCORE "sv-score"
#define SNSR_SV_THRESHOLD "sv-threshold"
#define SNSR_RES_SIGNAL_ENERGY "signal-energy"
#define SNSR_RES_NOISE_ENERGY "noise-energy"
#define SNSR_RES_SNR "snr"
#define SNSR_SCORE_OFFSET "score-offset"
#define SNSR_RES_TEXT "text"
#define SNSR_PHRASE_LIST "phrase-iterator"
#define SNSR_WORD_LIST "word-iterator"
#define SNSR_PHONE_LIST "phone-iterator"
#define SNSR_MODEL_LIST "model-iterator"
#define SNSR_ENROLL "enroll"
#define SNSR_ACCURACY "accuracy"
#define SNSR_INTERACTIVE_MODE "interactive"
#define SNSR_USER "user"
#define SNSR_DELETE_USER "delete-user"
#define SNSR_RE_ADAPT "re-adapt"
#define SNSR_RES_PERCENT_DONE "percent-done"
#define SNSR_RES_REASON "reason"
#define SNSR_RES_GUIDANCE "reason-guidance"
#define SNSR_RES_REASON_PASS "reason-pass"
#define SNSR_RES_REASON_VALUE "reason-value"
#define SNSR_RES_REASON_THRESHOLD "reason-threshold"
#define SNSR_RES_ENROLLMENT_COUNT "enrollment-count"
#define SNSR_RES_ENROLLMENT_ID "enrollment-id"
#define SNSR_ENROLLMENT_TARGET "req-enroll"
#define SNSR_ENROLLMENT_CONTEXT "ctx-enroll"
#define SNSR_ENROLLMENT_TASK_INDEX "enrollment-task-index"
#define SNSR_ADD_CONTEXT "add-context"
#define SNSR_SAVE_ENROLLMENT_AUDIO "save-enroll-audio"
#define SNSR_MODEL_STREAM "model-stream"
#define SNSR_AUDIO_STREAM "audio-stream"
#define SNSR_DONE_EVENT "^done"
#define SNSR_ENROLLED_EVENT "^enrolled"
#define SNSR_ADAPTED_EVENT "^adapted"
#define SNSR_FAIL_EVENT "^fail"
#define SNSR_PASS_EVENT "^pass"
#define SNSR_NEXT_EVENT "^next"
#define SNSR_PAUSE_EVENT "^pause"
#define SNSR_PROG_EVENT "^progress"
#define SNSR_RESUME_EVENT "^resume"
#define SNSR_ENROLLMENT_LIST "enrollment-iterator"
#define SNSR_REASON_LIST "reason-iterator"
#define SNSR_USER_LIST "user-iterator"
#include <stddef.h>
#define SNSR_NAME          "TrulyHandsfree"
#define SNSR_VERSION       "5.0.0-beta.10.2"
#define SNSR_VERSION_MAJOR  5
#define SNSR_VERSION_MINOR  0
#define SNSR_VERSION_PATCH  0
#define SNSR_VERSION_PRE   "beta.10.2"
#define SNSR_VERSION_BUILD ""
#define SNSR_VERSION_ID     22
#define SNSR_TECH_LEVEL    "5.0"
#define SNSR_SETTING_SZ     64
typedef struct SnsrSession_ *SnsrSession;
typedef enum {
  SNSR_FM_CONFIG,
  SNSR_FM_RUNTIME
} SnsrDataFormat;
typedef enum {
  SNSR_RC_OK,
  SNSR_RC_EOF,
  SNSR_RC_ERROR,
  SNSR_RC_NOT_OPEN,
  SNSR_RC_NOT_FOUND,
  SNSR_RC_NO_MEMORY,
  SNSR_RC_WRONG_MODE,
  SNSR_RC_INTERRUPTED,
  SNSR_RC_INVALID_ARG,
  SNSR_RC_INVALID_MODE,
  SNSR_RC_CANNOT_REOPEN,
  SNSR_RC_INVALID_HANDLE,
  SNSR_RC_NOT_IMPLEMENTED,
  SNSR_RC_DELIM_NOT_FOUND,
  SNSR_RC_FORMAT_NOT_SUPPORTED,
  SNSR_RC_BUFFER_OVERRUN,
  SNSR_RC_BUFFER_UNDERRUN,
  SNSR_RC_RESERVED_A,
  SNSR_RC_ARG_OUT_OF_RANGE,
  SNSR_RC_CHANNEL_EXISTS,
  SNSR_RC_CHANNEL_NOT_BUFFERED,
  SNSR_RC_CHANNEL_NOT_FOUND,
  SNSR_RC_CHANNELS_NOT_COMPATIBLE,
  SNSR_RC_CONFIGURATION_INCONSISTENT,
  SNSR_RC_CONFIGURATION_MISSING,
  SNSR_RC_CONNECTION_NOT_FOUND,
  SNSR_RC_DST_CHANNEL_NOT_FOUND,
  SNSR_RC_DST_ELEMENT_NOT_IN_BIN,
  SNSR_RC_DST_PORT_IN_USE,
  SNSR_RC_ELEMENT_ID_NOT_KNOWN,
  SNSR_RC_ELEMENT_INIT_FAILED,
  SNSR_RC_ELEMENT_INIT_INCOMPLETE,
  SNSR_RC_ELEMENT_IS_NOT_A_BIN,
  SNSR_RC_ELEMENT_NOT_IN_BIN,
  SNSR_RC_ELEMENT_NOT_ROOT_BIN,
  SNSR_RC_ELEMENT_REGISTRATION_FAILED,
  SNSR_RC_INCORRECT_SETTING_TYPE,
  SNSR_RC_LICENSE_NOT_VALID,
  SNSR_RC_ITERATION_LIMIT,
  SNSR_RC_ELEMENT_API_VIOLATION,
  SNSR_RC_NAME_NOT_UNIQUE,
  SNSR_RC_NOT_ENOUGH_SPACE,
  SNSR_RC_NOT_INITIALIZED,
  SNSR_RC_PUSH_FAILED,
  SNSR_RC_REPEAT,
  SNSR_RC_SETTING_FAILED,
  SNSR_RC_SETTING_IS_READ_ONLY,
  SNSR_RC_SETTING_NOT_AVAILABLE,
  SNSR_RC_SETTING_NOT_FOUND,
  SNSR_RC_SKIP,
  SNSR_RC_SRC_CHANNEL_NOT_FOUND,
  SNSR_RC_SRC_ELEMENT_NOT_IN_BIN,
  SNSR_RC_SRC_PORT_IN_USE,
  SNSR_RC_STOP,
  SNSR_RC_STREAM,
  SNSR_RC_STREAM_END,
  SNSR_RC_UNKNOWN_OBJECT_TYPE,
  SNSR_RC_VALUE_NOT_SET,
  SNSR_RC_RESERVED_B,
  SNSR_RC_NO_MODEL,
  SNSR_RC_REQUIRE_MISMATCH,
  SNSR_RC_VERSION_MISMATCH,
  SNSR_RC_LIBRARY_HEADER,
  SNSR_RC_LIBRARY_TOO_OLD,
  SNSR_RC_TIMED_OUT,
} SnsrRC;
typedef struct SnsrCallback_ *SnsrCallback;
typedef SnsrRC (*SnsrHandler)(SnsrSession s, const char *key, void *data);
typedef void (*SnsrDataRelease)(const void *data);
SnsrCallback
snsrCallback(SnsrHandler h, SnsrDataRelease r, void *data);
typedef struct SnsrStream_  *SnsrStream;
typedef enum {
  SNSR_ST_META_BYTES_READ,
  SNSR_ST_META_BYTES_WRITTEN,
  SNSR_ST_META_IS_OPEN,
  SNSR_ST_META_IS_READABLE,
  SNSR_ST_META_IS_WRITABLE,
  SNSR_ST_META_OPEN_COUNT,
} SnsrStreamMeta;
int
snsrStreamAtEnd(SnsrStream b);
SnsrRC
snsrStreamClose(SnsrStream b);
size_t
snsrStreamCopy(SnsrStream dst, SnsrStream src, size_t sizeInBytes);
const char *
snsrStreamErrorDetail(SnsrStream b);
size_t
snsrStreamGetDelim(SnsrStream b, void *buffer, size_t bufferSize, int delim);
size_t
snsrStreamGetMeta(SnsrStream b, SnsrStreamMeta key);
SnsrRC
snsrStreamOpen(SnsrStream b);
size_t
snsrStreamPrint(SnsrStream b, const char *format, ...);
SnsrRC
snsrStreamRC(SnsrStream b);
size_t
snsrStreamRead(SnsrStream a, void *buffer, size_t size, size_t nitems);
size_t
snsrStreamSkip(SnsrStream a, size_t size, size_t nitems);
size_t
snsrStreamWrite(SnsrStream a, const void *buffer, size_t size, size_t nitems);
typedef SnsrRC (*SnsrStreamEvent)(SnsrStream s, void *data);
typedef enum {
  SNSR_ST_AF_DEFAULT,
  SNSR_ST_AF_DEFAULT_LOW_LATENCY,
  SNSR_ST_AF_LPCM_S16_16K,
  SNSR_ST_AF_LPCM_S16_16K_LOW_LATENCY,
  SNSR_ST_AF_DEVICE,
  SNSR_ST_AF_DEVICE_LOW_LATENCY,
  SNSR_ST_AF_DEVICE_RATE_MODE,
  SNSR_ST_AF_DEVICE_RATE_MODE_LOW_LATENCY,
  SNSR_ST_AF_DEVID,
  SNSR_ST_AF_DEVID_LOW_LATENCY,
  SNSR_ST_AF_DEVID_RATE_MODE,
  SNSR_ST_AF_DEVID_RATE_MODE_LOW_LATENCY,
} SnsrStreamAudioFormat;
typedef enum {
  SNSR_ST_MODE_READ,
  SNSR_ST_MODE_WRITE,
} SnsrStreamMode;
#if defined(__ANDROID__) || defined(DOXYGEN)
#include <android/asset_manager_jni.h>
SnsrStream
snsrStreamFromAsset(AAssetManager *mgr, const char *filename);
#endif
#include <stdio.h>
SnsrStream
snsrStreamFromFILE(FILE *handle, SnsrStreamMode mode);
SnsrStream
snsrStreamFromFileName(const char *filename, const char *mode);
SnsrStream
snsrStreamFromAudioDevice(SnsrStreamAudioFormat format, ...);
SnsrStream
snsrStreamFromAudioStream(SnsrStream a, SnsrStreamAudioFormat format, ...);
SnsrStream
snsrStreamFromBuffer(size_t initialSizeInBytes, size_t maxSizeInBytes);
SnsrStream
snsrStreamFromMemory(void *buffer, size_t bufferSize, SnsrStreamMode mode);
SnsrStream
snsrStreamFromOpenStream(SnsrStream source, size_t sizeInBytes);
SnsrStream
snsrStreamFromStreams(SnsrStream a, SnsrStream b);
SnsrStream
snsrStreamFromString(const char *string);
SnsrStream
snsrStreamRaise(SnsrStreamEvent e, SnsrDataRelease r, void *data);
typedef struct {
  const char *name;
  SnsrRC (*open)(SnsrStream b);
  SnsrRC (*close)(SnsrStream b);
  void (*release)(SnsrStream b);
  size_t (*read)(SnsrStream b, void *data, size_t sizeInBytes);
  size_t (*write)(SnsrStream b, const void *data, size_t sizeInBytes);
} SnsrStream_Vmt;
SnsrStream
snsrStream_alloc(SnsrStream_Vmt *def, void *data, int readable, int writable);
void *
snsrStream_getData(SnsrStream b);
SnsrStream_Vmt *
snsrStream_getVmt(SnsrStream b);
void
snsrStream_setRC(SnsrStream h, SnsrRC code);
void
snsrStream_setDetail(SnsrStream b, const char *format, ...);
void
snsrClearRC(SnsrSession s);
void
snsrDescribeError(SnsrSession s, const char *format, ...);
SnsrRC
snsrDup(SnsrSession s, SnsrSession *dst);
const char *
snsrErrorDetail(SnsrSession s);
SnsrRC
snsrForEach(SnsrSession s, const char *key, SnsrCallback c);
SnsrRC
snsrGetDouble(SnsrSession s, const char *key, double *value);
SnsrRC
snsrGetInt(SnsrSession s, const char *key, int *value);
SnsrRC
snsrGetStream(SnsrSession s, const char *key, SnsrStream *stream);
SnsrRC
snsrGetString(SnsrSession s, const char *key, const char **value);
SnsrRC
snsrLoad(SnsrSession s, SnsrStream inputStream);
#define snsrNew(s) snsrNewValidateVersion((s), SNSR_VERSION)
SnsrRC
snsrNewValidateVersion(SnsrSession *s, const char *version);
SnsrRC
snsrRequire(SnsrSession s, const char *key, const char *value);
SnsrRC
snsrRun(SnsrSession s);
SnsrRC
snsrRC(SnsrSession s);
const char *
snsrRCMessage(SnsrRC returnCode);
void
snsrRetain(const void *handle);
void
snsrRelease(const void *handle);
SnsrRC
snsrSave(SnsrSession s, SnsrDataFormat format, SnsrStream outputStream);
SnsrRC
snsrSet(SnsrSession s, const char *keyValue);
SnsrRC
snsrSetDouble(SnsrSession s, const char *key, double value);
SnsrRC
snsrSetHandler(SnsrSession s, const char *key, SnsrCallback c);
SnsrRC
snsrSetInt(SnsrSession s, const char *key, int value);
SnsrRC
snsrSetStream(SnsrSession s, const char *key, SnsrStream stream);
SnsrRC
snsrSetString(SnsrSession s, const char *key, const char *value);
#ifdef _WIN32
int
getopt(int argc, char *const argv[], const char *optstr);
#endif
#if defined(__cplusplus)
}
#endif
#endif
