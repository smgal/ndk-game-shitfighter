package com.avej.game;

public class SmJNI
{
    static
    {
        System.loadLibrary("native-lib");
    }

    final static int ACTION_TYPE_TOUCH_DOWN = 1;
    final static int ACTION_TYPE_TOUCH_UP   = 2;
    final static int ACTION_TYPE_KEY_DOWN   = 3;
    final static int ACTION_TYPE_KEY_UP     = 4;

    /* **********************************************************
     * << Life cycle >>
     *
     * app launching: init -> resize -> render
     * app shutdown:  pause -> done
     * app pausing:   pause
     * app resuming:  resume -> init -> resize -> render
     *
     */
    public static native boolean init(String package_path, String data_path, String package_name, String app_name, SmType.SystemDesc system_desc);
    //public static native boolean init(String package_path);
    public static native void done();
    public static native void resize(int w, int h);
    public static native void render();
    public static native void pause();
    public static native void resume();
    public static native void process(int action, float x, float y);
}
