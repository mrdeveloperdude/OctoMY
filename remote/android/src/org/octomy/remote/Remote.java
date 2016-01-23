package org.octomy.remote;

import android.app.Notification;
import android.app.NotificationManager;
import android.content.Context;
import android.util.Log;
import android.widget.Toast;


public class Remote extends org.qtproject.qt5.android.bindings.QtActivity {
	private static NotificationManager m_notificationManager;
	private static Notification.Builder m_builder;
	private static Remote m_instance;
	private static String TAG="OCTOMY-REMOTE";

	public Remote() {
		m_instance = this;
		Log.d(TAG,"CTOR");
	}

	public static void notify(String s) {
		Log.d(TAG,"Notify "+s);
		if (m_notificationManager == null) {
			m_notificationManager = (NotificationManager)m_instance.getSystemService(Context.NOTIFICATION_SERVICE);
			m_builder = new Notification.Builder(m_instance);
	//		m_builder.setSmallIcon(R.drawable.icon);
			m_builder.setContentTitle("A message from OctoMY");
		}
		m_builder.setContentText(s);
		m_notificationManager.notify(1, m_builder.build());
	}

	public static void toast(String s){
		Context context = m_instance;//.getApplicationContext();
		int duration = Toast.LENGTH_SHORT;
		Toast toast = Toast.makeText(context, s, duration);
		toast.show();
	}

}
