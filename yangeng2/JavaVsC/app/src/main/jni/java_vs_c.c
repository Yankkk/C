#include <stdio.h>
#include <jni.h>
#include <string.h>

// Todo #1 generate the .h file to get the correct function name:
jstring  Java_what_should_go_here_I_wonder_see_hint_below
          (JNIEnv *env, jclass clas)
{
    return (*env)->NewStringUTF(env,"Hello From C!");
}

// You can create function declarations using javah
// cd into main/java and then just run javah edu.illinois.cs241.javavsc.CGlue to create a .h file
// which will include the correct function header

// Todo #2 implement your findMin
// (and use the .h file to get the header correct)

// Hint You will need to learn about how to access Java's float arrays as a simple float* array.


JNIEXPORT jstring JNICALL Java_edu_illinois_cs241_javavsc_CGlue_hello
  (JNIEnv * env, jclass CGlue)
{
    return  (*env)->NewStringUTF(env, "Hello world!!!");

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