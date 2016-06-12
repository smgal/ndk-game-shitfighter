package com.avej.game;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

import android.content.res.Configuration;
import android.graphics.Rect;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.app.Activity;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.Toast;

import com.avej.game.shitfighter.BuildConfig;
import com.avej.game.shitfighter.R;

import java.io.File;

import static com.avej.game.SmUtil.LOGI;

public class MainActivity extends Activity
{
	private MainView m_main_view;

	private char _prev_control_char_w = ' ';
	private char _prev_control_char_h = ' ';

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

		ImageView sliderHeightDirection = (ImageView)findViewById(R.id.slider_height_direction);
		sliderHeightDirection.setOnTouchListener
		(
			new View.OnTouchListener()
			{
				@Override
				public boolean onTouch(View v, MotionEvent event)
				{
					Rect rect = new Rect();
					v.getHitRect(rect);

					final float center_x = (rect.right + rect.left) / 2.0f;
					final float center_y = (rect.bottom + rect.top) / 2.0f;

					int action = event.getAction();

					if (action == MotionEvent.ACTION_CANCEL)
						action = MotionEvent.ACTION_UP;
					if (action == MotionEvent.ACTION_OUTSIDE)
						action = MotionEvent.ACTION_OUTSIDE;

					char control_char = (event.getRawY() < center_y) ? '8' : '2';

					if (_prev_control_char_h != ' ' && _prev_control_char_h != control_char)
					{
						SmUtil.processTouchButton(MotionEvent.ACTION_UP, _prev_control_char_h);
						SmUtil.processTouchButton(MotionEvent.ACTION_DOWN, control_char);
					}

					_prev_control_char_h = (action != MotionEvent.ACTION_UP) ? control_char : ' ';

					if (SmUtil.processTouchButton(action, control_char))
						return true;

					return false;
				}
			}
		);

		ImageView sliderWidthDirection = (ImageView)findViewById(R.id.slider_width_direction);
		sliderWidthDirection.setOnTouchListener
		(
			new View.OnTouchListener()
			{
				@Override
				public boolean onTouch(View v, MotionEvent event)
				{
					Rect rect = new Rect();
					v.getHitRect(rect);

					final float center_x = (rect.right + rect.left) / 2.0f;
					final float center_y = (rect.bottom + rect.top) / 2.0f;

					int action = event.getAction();

					if (action == MotionEvent.ACTION_CANCEL)
						action = MotionEvent.ACTION_UP;
					if (action == MotionEvent.ACTION_OUTSIDE)
						action = MotionEvent.ACTION_OUTSIDE;

					char control_char = (event.getRawX() < center_x) ? '4' : '6';

					if (_prev_control_char_w != ' ' && _prev_control_char_w != control_char)
					{
						SmUtil.processTouchButton(MotionEvent.ACTION_UP, _prev_control_char_w);
						SmUtil.processTouchButton(MotionEvent.ACTION_DOWN, control_char);
					}

					_prev_control_char_w = (action != MotionEvent.ACTION_UP) ? control_char : ' ';

					if (SmUtil.processTouchButton(action, control_char))
						return true;

					return false;
				}
			}
		);
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

	private void _init()
	{
		m_renderer = new MainRenderer();

		this.setEGLContextClientVersion(1);
		this.setPreserveEGLContextOnPause(true);

		this.setRenderer(m_renderer);
	}
	
	public MainView(Context context)
	{
		super(context);
		_init();
	}

	public MainView(Context context, AttributeSet attrib_set, int defStyle)
	{
		super(context, attrib_set);
		_init();
	}

	public MainView(Context context, AttributeSet attrib_set) {

		super(context, attrib_set);
		_init();
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
	private boolean _is_first = true;

	public void onSurfaceCreated(GL10 gl, EGLConfig config)
	{
		if (_is_first)
		{
			boolean app_found = SmJNI.init(SmConfig.PACKAGE_APK_PATH, SmConfig.PACKAGE_DATA_PATH, SmConfig.PACKAGE_NAME, SmConfig.APP_NAME, SmRes.system_desc);

			if (!app_found) {
				LOGI("<" + SmConfig.APP_NAME + "> not found");

				Toast.makeText(SmRes.context, "<" + SmConfig.APP_NAME + "> not found", Toast.LENGTH_LONG).show();
				SmRes.main_activity.finish();

				return;
			}

			SmConfig.BUFFER_WIDTH = SmRes.system_desc.buffer_width;
			SmConfig.BUFFER_HEIGHT = SmRes.system_desc.buffer_height;

			LOGI("Screen resolution is (" + String.valueOf(SmRes.system_desc.screen_width) + " x " + String.valueOf(SmRes.system_desc.screen_height) + ")");
			LOGI("Buffer size is (" + String.valueOf(SmRes.system_desc.buffer_width) + " x " + String.valueOf(SmRes.system_desc.buffer_height) + ")");

			_is_first = false;
		}
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
		for (int ix = 0; ix < SmRes.input_buffer.key_pressed.size(); ix++)
		{
			int key = SmRes.input_buffer.key_pressed.get(ix);
			SmJNI.process(SmJNI.ACTION_TYPE_KEY_DOWN, key, 0);
		}

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
/*
			case KEY_DOWN:
				SmJNI.process(SmJNI.ACTION_TYPE_KEY_DOWN, SmRes.input_buffer.key, 0);
				//result = SmJNI.process(time_ms, SmJNI.ACTION_TYPE_KEY_DOWN, SmRes.input_buffer.key, 0);
				break;

			case KEY_UP:
				SmJNI.process(SmJNI.ACTION_TYPE_KEY_UP, SmRes.input_buffer.key, 0);
				//result = SmJNI.process(time_ms, SmJNI.ACTION_TYPE_KEY_UP, SmRes.input_buffer.key, 0);
				break;
*/
		}

		SmRes.input_buffer.reset();

		if (!result)
			SmRes.is_terminating = true;
	}
}

