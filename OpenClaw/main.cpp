//#define SDL_MAIN_HANDLED
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include "Engine/GameApp/MainLoop.h"
#include "ClawGameApp.h"

#ifdef ANDROID
#include <jni.h>
#endif

ClawGameApp testApp;

int main(int argc, char* argv[]);

#ifdef ANDROID
extern "C" void SDL_Android_Init(JNIEnv* env, jclass cls);

// Start up the SDL app
extern "C" void Java_org_libsdl_app_SDLActivity_nativeInit( \
    JNIEnv* env, jclass cls, jobject obj) {
    /* This interface could expand with ABI negotiation, calbacks, etc. */
    SDL_Android_Init(env, cls);

    SDL_SetMainReady();

    /* Run the application code! */
    int status;
    char *argv[2];
    argv[0] = strdup("SDL_app");
    argv[1] = NULL;
    status = main(1, argv);

/* Do not issue an exit or the whole application will terminate instead of just the SDL thread */
//exit(status);
}
#endif //ANDROID

int main(int argc, char* argv[])
{
    return RunGameEngine(argc, argv);
}
