package com.avej.game;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.res.Configuration;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;
import android.widget.Toast;

import com.avej.game.shitfighter.BuildConfig;
import com.avej.game.shitfighter.R;

import java.io.File;

import static com.avej.game.SmUtil.LOGI;

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

			SmRes.src_rect.set(0, 0, SmRes.system_desc.buffer_width, SmRes.system_desc.buffer_height);
			SmRes.dst_rect.set(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			SmRes.scaling_factor_x = 1.0 * SmRes.src_rect.width() / SmRes.dst_rect.width();
			SmRes.scaling_factor_y = 1.0 * SmRes.src_rect.height() / SmRes.dst_rect.height();
		}

		setContentView(R.layout.main_activity);
		m_main_view = (MainView)findViewById(R.id.main_view);
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

	public MainView(Context context, AttributeSet attrib_set, int defStyle)
	{
		super(context, attrib_set);

		m_renderer = new MainRenderer();
		setRenderer(m_renderer);
	}

	public MainView(Context context, AttributeSet attrib_set) {

		super(context, attrib_set);

		m_renderer = new MainRenderer();
		setRenderer(m_renderer);
	}

	public boolean onTouchEvent(final MotionEvent event)
	{
		int motion_event = event.getAction();

		if (motion_event == MotionEvent.ACTION_DOWN)
		{
			int ax = (int)event.getX();
			int ay = (int)event.getY();

			SmRes.input_buffer.press(ax, ay);
		}
		else if (motion_event == MotionEvent.ACTION_MOVE)
		{
			int ax = (int)event.getX();
			int ay = (int)event.getY();

			SmRes.input_buffer.press(ax, ay);
		}
		else if (motion_event == MotionEvent.ACTION_UP)
		{
			SmRes.input_buffer.release();
		}

		return true;
/*
		int action = event.getAction();

		int x = (int)event.getX();
		int y = (int)event.getY();

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
*/
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
		this.process();
		SmJNI.render();
	}

	public void process()
	{
		if (SmRes.input_buffer.action_type == SmRes.InputBuffer.ACTION_TYPE.NONE)
			return;

		boolean result = false;

		long time_ms = System.currentTimeMillis() - SmRes.start_time;

		switch (SmRes.input_buffer.action_type)
		{
			case TOUCH_DOWN:
				int revised_touch_x = SmRes.input_buffer.touch_x;
				int revised_touch_y = SmRes.input_buffer.touch_y;

				if (revised_touch_x >= 0 && revised_touch_y >= 0 )
				{
					revised_touch_x -= SmRes.dst_rect.left;
					revised_touch_y -= SmRes.dst_rect.top;

					revised_touch_x = (int)((double)revised_touch_x * SmRes.scaling_factor_x);
					revised_touch_y = (int)((double)revised_touch_y * SmRes.scaling_factor_y);
				}

				SmJNI.process(SmJNI.ACTION_TYPE_TOUCH_DOWN, revised_touch_x, revised_touch_y);
				//result = SmJNI.process(time_ms, SmJNI.ACTION_TYPE_TOUCH_DOWN, revised_touch_x, revised_touch_y);
				break;

			case TOUCH_UP:
				SmJNI.process(SmJNI.ACTION_TYPE_TOUCH_UP, 0, 0);
				//result = SmJNI.process(time_ms, SmJNI.ACTION_TYPE_TOUCH_UP, 0, 0);
				break;

			case KEY_DOWN:
				SmJNI.process(SmJNI.ACTION_TYPE_KEY_DOWN, SmRes.input_buffer.key, 0);
				//result = SmJNI.process(time_ms, SmJNI.ACTION_TYPE_KEY_DOWN, SmRes.input_buffer.key, 0);
				break;

			case KEY_UP:
				SmJNI.process(SmJNI.ACTION_TYPE_KEY_UP, SmRes.input_buffer.key, 0);
				//result = SmJNI.process(time_ms, SmJNI.ACTION_TYPE_KEY_UP, SmRes.input_buffer.key, 0);
				break;
		}

		SmRes.input_buffer.reset();

		if (!result)
			SmRes.is_terminating = true;
	}
}
