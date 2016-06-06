package com.avej.game;

import android.content.Context;
import android.content.res.AssetManager;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.MotionEvent;
import android.view.View;
import android.widget.TextView;
import android.widget.Toast;

import com.avej.game.shitfighter.BuildConfig;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

public class SmUtil
{
	public final static Lock mutex = new ReentrantLock(true);

	public static void LOGD(String s)
	{
		if (BuildConfig.DEBUG)
			android.util.Log.d("[AVEJ_JAVA]", s);
	}

	public static void LOGI(String s)
	{
		android.util.Log.i("[AVEJ_JAVA]", s);
	}

	public static boolean processTouchButton(int action, char key)
	{
		if (action == MotionEvent.ACTION_DOWN)
			SmRes.input_buffer.keyDown(key);
		else if (action == MotionEvent.ACTION_UP)
			SmRes.input_buffer.keyUp(key);
		else
			return false;

		return true;
	}

	public static void copyAssets(Context context, String fileInAsset, String fileToCopy)
	{
		AssetManager assetManager = context.getAssets();
		InputStream in = null;
		OutputStream out = null;
		try
		{
			in = assetManager.open(fileInAsset);
			out = new FileOutputStream(fileToCopy);

			{
				byte[] buffer = new byte[8192];
				int readBytes;

				while ((readBytes = in.read(buffer)) >= 0)
				{
					out.write(buffer, 0, readBytes);
				}
			}

			in.close();
			in = null;

			out.flush();
			out.close();
			out = null;
		}
		catch(IOException e)
		{
			Log.e("[AVEJ_JAVA]", "Copy failed: " + fileInAsset + " -> " + fileToCopy, e);
		}
	}
}
