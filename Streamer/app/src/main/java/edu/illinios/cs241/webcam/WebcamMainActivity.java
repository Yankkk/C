package edu.illinios.cs241.webcam;

import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;

import java.net.Socket;


public class WebcamMainActivity extends ActionBarActivity {

    private static final String TAG="WebcamMainActivity" ;
    static {
        System.loadLibrary("webcamserverlibrary");
    }

    MyCameraPreview mCameraPreview;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.d(TAG,"onCreate");
        super.onCreate(savedInstanceState);
        //setContentView(R.layout.activity_main);
        mCameraPreview = new MyCameraPreview(this,null);
        setContentView(mCameraPreview);

        nativeStartServer();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mCameraPreview = null;
        nativeStopServer();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    // callback used by C code
    byte[] getImageBytes() {

        MyCameraPreview c = mCameraPreview;
        if(c == null) return null;
        return c.getRecentImageBytes();
    }
    private native int nativeStartServer();
    private native void nativeStopServer();
/*
    final int port = nativeStartServer();

    Thread t = new Thread() {

        public void run() {
            try {
                // Assumes IP4
                Socket s = new Socket("www.illinois.edu", 80);
                final String addr= s.getLocalAddress().getHostAddress();


                Log.d(TAG,addr);
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        setTitle(addr+":"+port);
                    }
                });
                s.close();
            } catch (Exception ex) {
                Log.e(TAG, ex.getMessage(), ex);
            }
        }
    };
    setTitle("webcam port "+port);
    t.start();
    */

}
