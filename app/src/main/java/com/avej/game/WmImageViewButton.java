package com.avej.game;

import android.content.Context;
import android.graphics.Rect;
import android.media.Image;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.widget.ImageView;

public class WmImageViewButton extends ImageView
{
	public WmImageViewButton(Context context, AttributeSet attrs, int defStyle)
	{
		super(context, attrs, defStyle);
	}

	public WmImageViewButton(Context context, AttributeSet attrs)
	{
		super(context, attrs);
	}

	public WmImageViewButton(Context context)
	{
		super(context);
	}

	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		String sTag = (String)this.getTag();

		if (sTag.length() > 0)
			if (SmUtil.processTouchButton(event.getAction(), sTag.charAt(0)))
				return true;

		return super.onTouchEvent(event);
	}

	@Override
	protected void onFocusChanged(boolean gainFocus, int direction, Rect previouslyFocusedRect)
	{
		if (!gainFocus)
		{
			String sTag = (String)this.getTag();
			SmUtil.processTouchButton(MotionEvent.ACTION_UP, sTag.charAt(0));
		}

		super.onFocusChanged(gainFocus, direction, previouslyFocusedRect);
	}
}
