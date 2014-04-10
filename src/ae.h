/* A simple event-driven programming library. Originally I wrote this code
 * for the Jim's event-loop (Jim is a Tcl interpreter) but later translated
 * it in form of a library for easy reuse.
 *
 * Copyright (c) 2006-2012, Salvatore Sanfilippo <antirez at gmail dot com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AE_H__
#define __AE_H__


#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4

#define AE_NOMORE -1

/* Macros */
#define AE_NOTUSED(V) ((void) V)

struct aeEventLoop;
/* Types and data structures */

typedef void aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);
typedef int aeTimeProc(struct aeEventLoop *eventLoop, long long id, void *clientData);
typedef void aeEventFinalizerProc(struct aeEventLoop *eventLoop, void *clientData);
typedef void aeBeforeSleepProc(struct aeEventLoop *eventLoop);

// File event structure

typedef struct aeFileEvent {
    int mask; /* one of AE_(READABLE|WRITABLE) */
    aeFileProc *rfileProc;
    aeFileProc *wfileProc;
    void *clientData;
} aeFileEvent;

/* Time event structure
 *
 * 鏃堕棿浜嬩欢缁撴瀯
 */
typedef struct aeTimeEvent {

    // 鏃堕棿浜嬩欢鐨勫敮涓�爣璇嗙
    long long id; /* time event identifier. */

    // 浜嬩欢鐨勫埌杈炬椂闂�
    long when_sec; /* seconds */
    long when_ms; /* milliseconds */

    // 浜嬩欢澶勭悊鍑芥暟
    aeTimeProc *timeProc;

    // 浜嬩欢閲婃斁鍑芥暟
    aeEventFinalizerProc *finalizerProc;

    // 澶氳矾澶嶇敤搴撶殑绉佹湁鏁版嵁
    void *clientData;

    // 鎸囧悜涓嬩釜鏃堕棿浜嬩欢缁撴瀯锛屽舰鎴愰摼琛�
    struct aeTimeEvent *next;

} aeTimeEvent;

/* A fired event
 *
 * 宸插氨缁簨浠�
 */
typedef struct aeFiredEvent {
    // 宸插氨缁枃浠舵弿杩扮
    int fd;
    // 浜嬩欢绫诲瀷鎺╃爜锛屽彲浠ユ槸 AE_READABLE 鎴�AE_WRITABLE
    int mask;
} aeFiredEvent;

/* State of an event based program 
 *
 * 浜嬩欢澶勭悊鍣ㄧ殑鐘舵�
 */
typedef struct aeEventLoop {
    int maxfd;   /* highest file descriptor currently registered */

    int setsize; /* max number of file descriptors tracked */

    long long timeEventNextId;

    time_t lastTime;     /* Used to detect system clock skew */

    aeFileEvent *events; /* Registered events */

    aeFiredEvent *fired; /* Fired events */

    aeTimeEvent *timeEventHead;

    int stop;

    void *apidata; /* This is used for polling API specific data */

    aeBeforeSleepProc *beforesleep;
} aeEventLoop;

aeEventLoop *aeCreateEventLoop(int setsize);
void aeDeleteEventLoop(aeEventLoop *eventLoop);
void aeStop(aeEventLoop *eventLoop);
int aeCreateFileEvent(aeEventLoop *eventLoop, int fd, int mask,
        aeFileProc *proc, void *clientData);
void aeDeleteFileEvent(aeEventLoop *eventLoop, int fd, int mask);
int aeGetFileEvents(aeEventLoop *eventLoop, int fd);
long long aeCreateTimeEvent(aeEventLoop *eventLoop, long long milliseconds,
        aeTimeProc *proc, void *clientData,
        aeEventFinalizerProc *finalizerProc);
int aeDeleteTimeEvent(aeEventLoop *eventLoop, long long id);
int aeProcessEvents(aeEventLoop *eventLoop, int flags);
int aeWait(int fd, int mask, long long milliseconds);
void aeMain(aeEventLoop *eventLoop);
char *aeGetApiName(void);
void aeSetBeforeSleepProc(aeEventLoop *eventLoop, aeBeforeSleepProc *beforesleep);

#endif
