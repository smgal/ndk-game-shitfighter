package com.avej.game;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Rect;
import android.media.MediaPlayer;

import java.util.ArrayList;

public class SmRes
{
	static Activity main_activity;
	static Context context;

	////////////////////////////////////////////////////////////////////////////
	// system variables

	static SmType.SystemDesc system_desc = new SmType.SystemDesc();

	static boolean is_terminating = false;
	// Atomic operation is guaranteed.
	static boolean is_rendering_queue_empty = true;

	static long    start_time = 0;

	////////////////////////////////////////////////////////////////////////////
	// public instances
/*
	static SmTask      game_task;
	static SmTaskAsync game_task_async;
	static MediaPlayer media_player;
*/
	////////////////////////////////////////////////////////////////////////////
	// main graphical buffer
/*
	static Bitmap  src_bitmap;
*/
	static Rect    src_rect = new Rect(0, 0, SmConfig.BUFFER_WIDTH, SmConfig.BUFFER_HEIGHT);
	static Rect    dst_rect = new Rect();
	static double  scaling_factor_x = 1.0;
	static double  scaling_factor_y = 1.0;

	////////////////////////////////////////////////////////////////////////////
	// touch event

	public static class InputBuffer
	{
		enum ACTION_TYPE
		{
			NONE,
			TOUCH_DOWN,
			TOUCH_UP,
			KEY_DOWN,
			KEY_UP
		};

		ACTION_TYPE action_type = ACTION_TYPE.NONE;

		//boolean just_touched = false;
		int     touch_x = -1;
		int     touch_y = -1;

		boolean just_pressed = false;
		int     key = 0;

		/////////////////////////

		ArrayList<Character> key_pressed = new ArrayList<Character>();

		void reset()
		{
			action_type = ACTION_TYPE.NONE;
		}

		void press(int x, int y)
		{
			action_type = ACTION_TYPE.TOUCH_DOWN;
			touch_x = x;
			touch_y = y;
			//just_touched = true;
		}

		void release()
		{
			action_type = ACTION_TYPE.TOUCH_UP;
			touch_x = -1;
			touch_y = -1;
			//just_touched = true;
		}

		void keyDown(char _key)
		{
			action_type = ACTION_TYPE.KEY_DOWN;
			key = (int)_key;

			if (!key_pressed.contains(_key))
				key_pressed.add(_key);
		}

		void keyUp(char _key)
		{
			action_type = ACTION_TYPE.KEY_UP;
			key = (int)_key;

			if (key_pressed.contains(_key))
				key_pressed.remove((Character)_key);
		}
	}

	static InputBuffer input_buffer = new InputBuffer();
}
