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