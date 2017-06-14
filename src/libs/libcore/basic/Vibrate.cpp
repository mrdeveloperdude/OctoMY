#include "Vibrate.hpp"

Vibrate::Vibrate()
{

}
/*
 *
 * FROM: https://forum.qt.io/topic/44995/solved-qt-on-android-how-to-vibrate-the-device/13
 *
List below steps for all developers.

change the .pro file in QT in this way:
@QT += core gui androidextras@

@ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android@

Add the permission in the AndroidMainifest.xml
@<uses-permission android:name="android.permission.VIBRATE"/>@

replace the attribute in the activity:
from

@android:name="org.qtproject.qt5.android.bindings.QtActivity"@

to

@android:name="org.qtproject.example.Chronometer.Vibrate"@

Where Chronometer and Vibrate string can be changes.

add the include following in the mainwindow.h
@#include <QtAndroidExtras/QAndroidJniObject>@

add the following Vibrate.java class in the following directory
myprogget>android/src/org/qtproject/example/Chronometer/

where Chronometer can be change:

@//
// Vibrate.java
//
package org.qtproject.example.Chronometer;

import android.content.Context;
import android.os.Vibrator;
import android.app.Activity;
import android.os.Bundle;

public class Vibrate extends org.qtproject.qt5.android.bindings.QtActivity
{

public static Vibrator m_vibrator;
public static Vibrate m_istance;

public Vibrate()
{
m_istance = this;
}

public static void start(int x)
{
if (m_vibrator == null)
{
if (m_istance != null)
{
m_vibrator = (Vibrator) m_istance.getSystemService(Context.VIBRATOR_SERVICE);
m_vibrator.vibrate(x);
}

   }
   else m_vibrator.vibrate(x);
}
}@

in your program call class in this way:
@QAndroidJniObject::callStaticMethod<void>("org/qtproject/example/Chronometer/Vibrate", "start", "(I)V", 300);@
    */
