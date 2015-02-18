#include <stdio.h>
#include <jni.h>
#include <string.h>
#include <android/log.h>

#define LOG_TAG "NdkC"
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


// Todo #1 generate the .h file to get the correct function name:
/**
jstring  Java_what_should_go_here_I_wonder_see_hint_below
          (JNIEnv *env, jclass clas)
{
    return (*env)->NewStringUTF(env,"Hello From C!");
}
*/

// You can create function declarations using javah
// cd into main/java and then just run javah edu.illinois.cs241.javavsc.CGlue to create a .h file
// which will include the correct function header

// Todo #2 implement your findMin
// (and use the .h file to get the header correct)

// Hint You will need to learn about how to access Java's float arrays as a simple float* array.


JNIEXPORT jstring JNICALL Java_edu_illinois_cs241_javavsc_CGlue_hello
  (JNIEnv * env, jclass CGlue)
{
    return  (*env)->NewStringUTF(env, "Hello From C!");

}


JNIEXPORT jfloat JNICALL Java_edu_illinois_cs241_javavsc_CGlue_findMin
  (JNIEnv * env, jclass CGlue, jfloatArray data)
{
    int len = (*env)-> GetArrayLength(env, data);


    //jfloatArray carr[len];
    //carr = (float*)malloc(len * sizeof(float));
    int i;
    jfloat * carr = (*env) -> GetFloatArrayElements(env, data, 0);

    float min = carr[0];
    for(i=0; i < len; i++){
        if(min > carr[i]){
            min = carr[i];
            }
    }
    (*env)->ReleaseFloatArrayElements(env, data, carr, 0);
    return min;
}
JNIEXPORT jint JNICALL Java_edu_illinois_cs241_javavsc_CGlue_count
  (JNIEnv * env, jclass CGlue, jfloatArray a, jfloatArray b)
  {
    LOGI("SETUP");
    int len = (*env)->GetArrayLength(env, a);
    int count = 0;
    int i;
    jfloat * ca = (*env) -> GetFloatArrayElements(env, a, 0);
    jfloat * cb = (*env) -> GetFloatArrayElements(env, b, 0);
    LOGE("SETUP FAILED");
    for(i = 0; i < len; i++){
        if(ca[i] >= cb[i])
            count++;

    }

    (*env)->ReleaseFloatArrayElements(env, a, ca, 0);
    (*env)->ReleaseFloatArrayElements(env, b, cb, 0);

    return count;
    LOGI("SETUP EXIT");
  }
