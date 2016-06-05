package com.avej.game;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.res.Configuration;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.avej.game.shitfighter.BuildConfig;

import java.io.File;

import static com.avej.game.SmUtil.LOGI;

class SmConfig
{
	static int BUFFER_WIDTH = 1280;
	static int BUFFER_HEIGHT = 720;

	static String PACKAGE_NAME;
	static String PACKAGE_APK_PATH;
	static String PACKAGE_DATA_PATH;
	static String APP_NAME;
}

class SmType
{
	public static class SystemDesc
	{
		int screen_width;
		int screen_height;
		int buffer_width;
		int buffer_height;
	}
}

class SmRes
{
	static Activity main_activity;
	static Context context;

	////////////////////////////////////////////////////////////////////////////
	// system variables

	static SmType.SystemDesc system_desc = new SmType.SystemDesc();
/*
	static boolean is_terminating = false;
	static long    start_time = 0;
*/
}

class SmUtil
{
	public static void LOGD(String s)
	{
		if (BuildConfig.DEBUG)
			android.util.Log.d("[AVEJ_JAVA]", s);
	}

	public static void LOGI(String s)
	{
		android.util.Log.i("[AVEJ_JAVA]", s);
	}
}

public class MainActivity extends Activity
{
	private MainView m_main_view;

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		SmRes.main_activity = this;
		SmRes.context = getApplicationContext();

		getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

		requestWindowFeature(Window.FEATURE_NO_TITLE);

		if (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE)
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
		else
			setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);

		{
			SmConfig.PACKAGE_NAME = getPackageName();
			SmConfig.PACKAGE_APK_PATH = getPackageResourcePath();

			File file = SmRes.context.getFilesDir();
			SmConfig.PACKAGE_DATA_PATH = file.getAbsolutePath();
		}

		SmConfig.APP_NAME = "shitfighter";

		{
			final int SCREEN_WIDTH = SmRes.context.getResources().getDisplayMetrics().widthPixels;
			final int SCREEN_HEIGHT = SmRes.context.getResources().getDisplayMetrics().heightPixels;

			SmRes.system_desc.screen_width  = SCREEN_WIDTH;
			SmRes.system_desc.screen_height = SCREEN_HEIGHT;
			SmRes.system_desc.buffer_width  = SmConfig.BUFFER_WIDTH;
			SmRes.system_desc.buffer_height = SmConfig.BUFFER_HEIGHT;
		}

		m_main_view = new MainView(this);
		setContentView(m_main_view);
	}

	@Override
	public void onDestroy()
	{
		super.onDestroy();
		SmJNI.done();
		android.os.Process.killProcess(android.os.Process.myPid());
	}

	@Override
	protected void onPause()
	{
		super.onPause();
		m_main_view.onPause();
	}
	
	@Override
	protected void onResume()
	{
		super.onResume();
		m_main_view.onResume();
	}
}

class MainView extends GLSurfaceView
{
	MainRenderer m_renderer;
	
	public MainView(Context context)
	{
		super(context);
		m_renderer = new MainRenderer();
		setRenderer(m_renderer);
	}

	public boolean onTouchEvent(final MotionEvent event)
	{
		int action = event.getAction();

		float x = event.getX();
		float y = event.getY();

		switch (action)
		{
		case MotionEvent.ACTION_DOWN:
			SmJNI.process(SmJNI.ACTION_TYPE_TOUCH_DOWN, x, y);
			break;
		case MotionEvent.ACTION_UP:
			SmJNI.process(SmJNI.ACTION_TYPE_TOUCH_UP, x, y);
			break;
		case MotionEvent.ACTION_MOVE:
			SmJNI.process(SmJNI.ACTION_TYPE_TOUCH_DOWN, x, y);
			break;
		case MotionEvent.ACTION_CANCEL:
			SmJNI.process(SmJNI.ACTION_TYPE_TOUCH_UP, x, y);
			break;
		}

		return true;
	}

	@Override
	public void onPause()
	{
		super.onPause();
		SmJNI.pause();
	}

	@Override
	public void onResume()
	{
		super.onResume();
		SmJNI.resume();
	}
}

class MainRenderer implements GLSurfaceView.Renderer
{
	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		boolean app_found = SmJNI.init(SmConfig.PACKAGE_APK_PATH, SmConfig.PACKAGE_DATA_PATH, SmConfig.PACKAGE_NAME, SmConfig.APP_NAME, SmRes.system_desc);
		//boolean app_found = SmJNI.init(SmConfig.PACKAGE_APK_PATH);

		if (!app_found)
		{
			LOGI("<" + SmConfig.APP_NAME + "> not found");

			Toast.makeText(SmRes.context, "<" + SmConfig.APP_NAME + "> not found", Toast.LENGTH_LONG).show();
			SmRes.main_activity.finish();

			return;
		}

		SmConfig.BUFFER_WIDTH  = SmRes.system_desc.buffer_width;
		SmConfig.BUFFER_HEIGHT = SmRes.system_desc.buffer_height;

		LOGI("Screen resolution is (" + String.valueOf(SmRes.system_desc.screen_width) + " x " + String.valueOf(SmRes.system_desc.screen_height) + ")");
		LOGI("Buffer size is (" + String.valueOf(SmRes.system_desc.buffer_width) + " x " + String.valueOf(SmRes.system_desc.buffer_height) + ")");
	}
	
	public void onSurfaceChanged(GL10 gl, int w, int h)
	{
		SmJNI.resize(w, h);
	}
	
	public void onDrawFrame(GL10 gl)
	{
		SmJNI.render();
	}
}
