package com.example.gjy.myapplication;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;

import com.webrtc.ns.AudioProcessJni;
import com.webrtc.ns.Test;
import com.webrtc.ns.TestAgc;
import com.webrtc.ns.TestNs;


public class MainActivity extends Activity {
    static{
        System.loadLibrary("webrtc_audio");  //加载native code的动态库
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //init

    }

    public void goToTese(View view) {

        new Thread(){

            @Override
            public void run() {

                //  TestAgc.agc_audio_test(MainActivity.this);
                TestNs.ns_audio_test(MainActivity.this);
            }
        }.start();

    }

    @Override
    public boolean onKeyDown(int keyCode, KeyEvent event) {
        return super.onKeyDown(keyCode, event);
    }
}
