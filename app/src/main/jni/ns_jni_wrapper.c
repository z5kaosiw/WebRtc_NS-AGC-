/* Note:Your choice is C IDE */
/* Includes ------------------------------------------------------------------*/
#include <stdlib.h>
#include  "stdio.h"
#include  "jni.h"
#include <string.h>
#include <android/log.h>
#include <agc/noise_suppression.h>
#include <agc/noise_suppression_x.h>
#include <agc/gain_control.h>
#include <agc/analog_agc.h>
#include <fcntl.h>

#define TAG "Android_JNI"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__)

#define NS_ERROR  -1
#define NS_SUCCESS  7
#define AGC_SUCCESS  7
static NsHandle  *pNS_inst = NULL;
//static 	NsxHandle *pNS_inst = NULL;
static Agc_t *agcHandle = NULL;
static int agc_samples = 8000;
unsigned  char* agc_buffer = NULL;
unsigned  char* ns_buffer = NULL;
void shortTobyte(int len , short *p , unsigned char *q){

    int i;
    for(i=0;i<len;i++)
    {
        q[2*i+1] = (unsigned char)(p[i]>>8);
        q[2*i] = p[i]&0x00ff;

    }
}

//初始化频率的降噪结构体参数
/*
 * nSample:采集数据的频率
 */
JNIEXPORT jint JNICALL Java_com_webrtc_ns_AudioProcessJni_initiateNSInstance(JNIEnv *env, jclass cls,  jlong nSample, jint nMode){

    //创建webrtc降噪处理句柄
    //NsHandle *hNS = NULL; //create a pointer to NsHandle on native stack.
    if ( (  WebRtcNs_Create(&pNS_inst) ) !=  0) { //allocate dynamic memory on native heap for NS instance pointed by hNS.

        LOGE("Noise_Suppression WebRtcNs_Create err! \n");
        return  NS_ERROR;  //error occurs

    }

    if (0 !=  WebRtcNs_Init(pNS_inst,nSample))
	{
		LOGE("Noise_Suppression WebRtcNs_Init err! \n");
		return  NS_ERROR;  //error occurs
	}

	if (0 !=  WebRtcNs_set_policy(pNS_inst,nMode))
	{
		printf("Noise_Suppression WebRtcNs_set_policy err! \n");
		return  NS_ERROR;  //error occurs
	}

   // return ((int) (NSinst_t *) pNS_inst); //returns the address of NS instance on native heap.
      return NS_SUCCESS;
}
JNIEXPORT jint JNICALL  Java_com_webrtc_ns_AudioProcessJni_processNS(JNIEnv *env, jclass type, jobject jdirectBuff,jshortArray outframe_,jint sf) {



    jshort *outframe = (*env)->GetShortArrayElements(env, outframe_, NULL);
    short *pOutData    = NULL;
    int frameSize =sf;

    if(pNS_inst == NULL){
        LOGE("nsHandle is null! \n");
       return  -3;
   }
    if(ns_buffer == NULL){
        LOGE("gc_buffer == NULL! \n");
        void* buffer = (*env)->GetDirectBufferAddress(env,jdirectBuff);
        ns_buffer = buffer;
    }
    pOutData = (short*)malloc(frameSize*sizeof(short));
    if ( WebRtcNs_Process(pNS_inst, outframe, NULL, pOutData,NULL) != 0){

        LOGE("failed in WebRtcAgc_Process! \n" );
        return  NS_ERROR ;  //error occurs

    }

    shortTobyte(80,pOutData,ns_buffer);
    (*env)->ReleaseShortArrayElements(env, outframe_, outframe, 0);

    return  AGC_SUCCESS;

}

JNIEXPORT jint JNICALL Java_com_webrtc_ns_AudioProcessJni_destoryNS(JNIEnv *env, jclass cls){

        WebRtcNs_Free(pNS_inst);
}
/*
 * Class:     com_webrtc_ns_AudioProcessJni
 * Method:    AgcInit
 * Signature: (JJJ)J
 */
JNIEXPORT jlong JNICALL Java_com_webrtc_ns_AudioProcessJni_AgcInit(JNIEnv *env, jclass cls, jlong minLevel , jlong maxLevel , jlong fs){

        minLevel = 0;
		maxLevel = 255;
		agc_samples  =fs;
		int agcMode  = kAgcModeFixedDigital;
        LOGE("Java_com_webrtc_ns_AudioProcessJni_AgcInit!  -> %d \n", sizeof(short));
        if ( (   WebRtcAgc_Create(&agcHandle) ) != 0) { //allocate dynamic memory on native heap for NS instance pointed by hNS.

             LOGE("Noise_Suppression WebRtcNs_Create err! \n");
             return  NS_ERROR;  //error occurs

        }
        LOGE("Java_com_webrtc_ns_AudioProcessJni_AgcCreate success! \n");
        if (0 !=  WebRtcAgc_Init(agcHandle, minLevel, maxLevel, agcMode, agc_samples) )
	    {
             LOGE("WebRtcAgc_Init WebRtcNs_Init err! \n");
             return  NS_ERROR;  //error occurs
	    }

      LOGE("Java_com_webrtc_ns_AudioProcessJni_AgcInit success! \n");
        WebRtcAgc_config_t agcConfig;
		agcConfig.compressionGaindB = 20; //在Fixed模式下，越大声音越大
		agcConfig.limiterEnable     = 1;
		agcConfig.targetLevelDbfs   = 3;  //dbfs表示相对于full scale的下降值，0表示full scale，越小声音越大
		WebRtcAgc_set_config(agcHandle, agcConfig);

		return NS_SUCCESS;
}

JNIEXPORT jint JNICALL Java_com_webrtc_ns_AudioProcessJni_AgcFun(JNIEnv *env, jclass type, jobject jdirectBuff,jshortArray sArr_, jint frameSize) {
    if(agc_buffer == NULL){
        LOGE("gc_buffer == NULL! \n");
        void* buffer = (*env)->GetDirectBufferAddress(env,jdirectBuff);
        agc_buffer = buffer;
    }
    uint8_t saturationWarning;
    int outMicLevel = 0;
    int micLevelOut = 0;
    int i =0 ;
    int inMicLevel  = micLevelOut;
    const short *pData    = NULL;
    short *pOutData    = NULL;
    pOutData = (short*)malloc(frameSize*sizeof(short));
    pData  =(*env)->GetShortArrayElements(env,sArr_,NULL);
    if(agcHandle == NULL){
        LOGE("agcHandle is null! \n");
        return  -3;
    }
    if(frameSize <= 0){
        return  -2;
    }
    int  agcProcessResult =  WebRtcAgc_Process(agcHandle,
                                               pData,
                                               NULL,
                                               frameSize,
                                               pOutData,
                                               NULL,
                                               inMicLevel,
                                               &outMicLevel,
                                               0,
                                               &saturationWarning);
    if (0 !=  agcProcessResult )
    {
        LOGE("failed in WebRtcAgc_Process!  agcProcessResult = %d \n" ,agcProcessResult);
        return  NS_ERROR ;  //error occurs
    }
    //memset(agc_buffer, 0,  160);
    shortTobyte(80,pOutData,agc_buffer);
    (*env)->ReleaseShortArrayElements(env, sArr_, pData, 0);
    return  AGC_SUCCESS;
}

JNIEXPORT void JNICALL Java_com_webrtc_ns_AudioProcessJni_AgcFree(JNIEnv *env , jclass  cls){

    WebRtcAgc_Free(agcHandle);
}
JNIEXPORT void JNICALL  Java_com_webrtc_ns_AudioProcessJni_AgcProcess(JNIEnv *env  , jclass type) {

    // TODO  test agc model
    const char *sFile     = "/storage/emulated/0/Pictures/agc/byby_8K_1C_16bit.pcm";
    const char *dFile     ="/storage/emulated/0/Pictures/agc/byby_8K_1C_16bit_agc.pcm";
    const char *txtFile1     ="/storage/emulated/0/Pictures/agc/jni_change_data_agc.txt";
    FILE * rf = fopen(sFile, "rb");
    FILE * wf = fopen(dFile, "wb");
    FILE * txtFile = fopen(txtFile1, "wb");
    if(rf == NULL || wf == NULL){

        LOGE("file open filed");
        return;
    }
    if (txtFile == NULL){

        LOGE("txtFile open filed");
        return;

    }
    int len;
    int frameSize =80;
    short *pData    = NULL;
    short *pOutData = NULL;
    pData    = (short*)malloc(frameSize*sizeof(short));
    pOutData = (short*)malloc(frameSize*sizeof(short)); //160
    len = frameSize*sizeof(short);
    int micLevelOut = 0;
    while(1)
    {
        memset(pData, 0, len);
        len = fread(pData, 1, len, rf);

                if (len > 0)
                {
                    int inMicLevel  = micLevelOut;
                    int outMicLevel = 0;
                    uint8_t saturationWarning;
                    int nAgcRet = WebRtcAgc_Process(agcHandle, pData, NULL, frameSize, pOutData,NULL, inMicLevel, &outMicLevel, 0, &saturationWarning);
                    if (nAgcRet != 0)
                    {
                        LOGE("failed in WebRtcAgc_Process\n");
                        break;
                    }
                  fwrite(pOutData, 1, len, wf);

                } else {
                        break;
                }
     }
        LOGE("agc done");
        fclose(wf);
        fclose(rf);
        fclose(txtFile);
        free(pData);
        free(pOutData);
        WebRtcAgc_Free(agcHandle);

}
/*JNI全处理*/
JNIEXPORT void JNICALL  Java_com_webrtc_ns_AudioProcessJni_NSProcess(JNIEnv *env, jclass type ,jint sf) {

    const char *sFile     = "/storage/emulated/0/Pictures/ns/lhydd_1C_16bit_32K.pcm";
    const char *dFile     ="/storage/emulated/0/Pictures/ns/lhydd_1C_16bit_32K_ns.pcm";
    const char *sFile_8k     = "/storage/emulated/0/Pictures/ns/byby_8K_1C_16bit.pcm";
    const char *dFile_8k     ="/storage/emulated/0/Pictures/ns/byby_8K_1C_16bit_ns.pcm";
    // TODO  test agc model
    char *pInBuffer = NULL;
    char *pOutBuffer = NULL;
    int i = 0;
    long nFileSize = 0;

    if(sf == 32*1000) {

        FILE * fpIn = fopen(sFile, "rb");
        FILE * fpOut = fopen(dFile, "wb");
        if(fpIn == NULL || fpOut == NULL){

            LOGE("file open filed");
            return;
        }

                LOGE("NS Java_com_webrtc_ns_AudioProcessJni_NSProcess  32K   start");
                do {

                    fseek(fpIn, 0, SEEK_END);
                    nFileSize = ftell(fpIn);
                    printf("nFileSize = %d  \n ", nFileSize);
                    fseek(fpIn, 0, SEEK_SET);

                    pInBuffer = (char *) malloc(nFileSize);
                    memset(pInBuffer, 0, nFileSize);
                    fread(pInBuffer, sizeof(char), nFileSize, fpIn);


                    pOutBuffer = (char *) malloc(nFileSize);
                    memset(pOutBuffer, 0, nFileSize);

                    int filter_state1[6], filter_state12[6];
                    int Synthesis_state1[6], Synthesis_state12[6];

                    memset(filter_state1, 0, sizeof(filter_state1));
                    memset(filter_state12, 0, sizeof(filter_state12));
                    memset(Synthesis_state1, 0, sizeof(Synthesis_state1));
                    memset(Synthesis_state12, 0, sizeof(Synthesis_state12));

                    for (i = 0; i < nFileSize; i += 640) {
                        if (nFileSize - i >= 640) {
                            short shBufferIn[320] = {0};

                            short shInL[160], shInH[160];
                            short shOutL[160] = {0}, shOutH[160] = {0};

                            memcpy(shBufferIn, (char *) (pInBuffer + i), 320 * sizeof(short));
                            //首先需要使用滤波函数将音频数据分高低频，以高频和低频的方式传入降噪函数内部
                            WebRtcSpl_AnalysisQMF(shBufferIn, 320, shInL, shInH, filter_state1,
                                                  filter_state12);

                            //将需要降噪的数据以高频和低频传入对应接口，同时需要注意返回数据也是分高频和低频
                            if (0 == WebRtcNs_Process(pNS_inst, shInL, shInH, shOutL, shOutH)) {
                                short shBufferOut[320];
                                //如果降噪成功，则根据降噪后高频和低频数据传入滤波接口，然后用将返回的数据写入文件
                                WebRtcSpl_SynthesisQMF(shOutL, shOutH, 160, shBufferOut, Synthesis_state1,
                                                       Synthesis_state12);
                                memcpy(pOutBuffer + i, shBufferOut, 320 * sizeof(short));
                            }
                        }
                    }

                    fwrite(pOutBuffer, sizeof(char), nFileSize, fpOut);


                } while (0);
                    LOGE("NS 结束");
                    WebRtcNs_Free(pNS_inst);
                    fclose(fpIn);
                    fclose(fpOut);
                    free(pInBuffer);
                    free(pOutBuffer);
    } else{

        FILE * fpIn = fopen(sFile_8k, "rb");
        FILE * fpOut = fopen(dFile_8k, "wb");
        if(fpIn == NULL || fpOut == NULL){

            LOGE("file open filed");
            return;
        }

        LOGE("NS Java_com_webrtc_ns_AudioProcessJni_NSProcess  8K   start");
        int len;
        int frameSize =80;
        int i = 0 ;
        const  short *pData    = NULL;
        short *pOutData = NULL;
        pData    = (short*)malloc(frameSize*sizeof(short));
        pOutData = (short*)malloc(frameSize*sizeof(short)); //160
        len = frameSize*sizeof(short);
        while(1)
        {
            memset(pData, 0, len);

            if (len > 0)
            {

                len = fread(pData, 1, len, fpIn);

                int nSRet = WebRtcNs_Process(pNS_inst, pData, NULL, pOutData,NULL);
                if (nSRet != 0)
                {
                    LOGE("failed in WebRtcAgc_Process\n");
                    break;
                }

                  fwrite(pOutData, 1, len, fpOut);

            } else {
                break;
            }
        }

        LOGE("NS 结束");
        WebRtcNs_Free(pNS_inst);
        fclose(fpIn);
        fclose(fpOut);
        free(pInBuffer);
        free(pOutBuffer);


    }

}

JNIEXPORT jint JNICALL  Java_com_webrtc_ns_AudioProcessJni_processNS32K(JNIEnv *env, jclass type, jobject buffer1,
                                                jshortArray outframe_, jint sf) {
    jshort *outframe = (*env)->GetShortArrayElements(env, outframe_, NULL);


    (*env)->ReleaseShortArrayElements(env, outframe_, outframe, 0);
}