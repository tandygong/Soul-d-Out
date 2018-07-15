#include <stdio.h>
#include <stdlib.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#ifdef ANDROID
#include <jni.h>
#include <android/log.h>
#include "common.h"

jint Java_com_jesse_soulout_util_VideoUtils_getVideoRotation(JNIEnv* env, jclass jobj, jstring filePath) {
    char* file_path = (*env)->GetStringUTFChars(env, filePath, NULL);
    LOGI("file_path: %s", file_path);
    // 1.初始化
    avcodec_register_all();
    AVFormatContext *pFormatCtx = avformat_alloc_context();
    // 2.打开文件
    if (!avformat_open_input(&pFormatCtx, file_path, NULL, NULL)) {
        LOGE("%s", "无法打开视频文件");
        return -1;
    }
    // 3.
    if (!avformat_find_stream_info(pFormatCtx, NULL)) {
        LOGE("%s", "无法获取视频文件信息");
        return -2;
    }
    // 4.寻找视频流
    int i = 0, v_stream_idx = -1;
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            v_stream_idx = i;
            break;
        }
    }
    if (v_stream_idx == -1) {
        LOGE("%s", "无法获取视频流");
        return -3;
    }
    // 5.读取角度信息
    AVDictionaryEntry *tag = NULL;
    tag = av_dict_get(pFormatCtx->streams[v_stream_idx]->metadata, "rotate", tag, 0);
    int angle = -1;
    if (tag) {
        angle = atoi(tag->value);
        LOGI("angle:%d", angle);
    }
    avformat_free_context(pFormatCtx);
    return angle;
}