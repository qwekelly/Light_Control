package com.example.led;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;
import static java.lang.System.out;
import static java.lang.System.runFinalization;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;

public class MainActivity extends AppCompatActivity {
    volatile Boolean flag_SRX =null;
    private Handler handler=null;
    private volatile Boolean flag=null;
    private Bundle bundle=null;
    private Button button=null;
    private Button mLedON=null;
    private Button mLedOFF=null;
    private EditText editText1=null;
    private EditText editText2=null;

    private Socket socket=null;
    private BufferedReader bufferedReader=null;
    private BufferedWriter bufferedWriter=null;

    @SuppressLint("HandlerLeak")
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final boolean loop = true;
        flag = false;
        flag_SRX = false;
        bundle = new Bundle();
        button = (Button) findViewById(R.id.mBtnConnect);
        mLedON = (Button) findViewById(R.id.mLedON);
        mLedOFF = (Button) findViewById(R.id.mLedOFF);
        editText1 = (EditText) findViewById(R.id.mIp);
        final EditText editText2 = (EditText) findViewById(R.id.mPort);

        handler = new Handler() {                         //测试子线程给主线程传输数据
            @Override
            public void handleMessage(@NonNull Message msg) {
                super.handleMessage(msg);
                Log.i("handler", "处理函数");
                if (msg.what == 0x11) {

                }runFinalization();
                if (msg.what == 0x22) button.setText("关闭连接");
            }
        };
        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
            if (!flag) {
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            char time = 0;
                            socket = new Socket(editText1.getText().toString(), Integer.valueOf(editText2.getText().toString()));
                            bufferedWriter = new BufferedWriter(new OutputStreamWriter(socket.getOutputStream()));
                            bufferedReader = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                            Log.i("开始循环等待！", "Warring");
                            while (socket.isConnected() == false) {
                                try {
                                    Thread.sleep(100);
                                    time++;
                                    if (time == 10) {
                                        Log.i("等待时间到，退出等待！", "Warring");
                                        break;
                                    }
                                } catch (InterruptedException e) {
                                    e.printStackTrace();
                                }
                            }
                            flag_SRX = true;
                            flag = true;
                            handler.sendEmptyMessage(0x22);
                        } catch (UnknownHostException e) {
                            e.printStackTrace();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            } else {
                try {
                    bufferedReader.close();
                    bufferedWriter.close();
                    socket.close();
                    Log.i("关闭连接！", "Warring");
                } catch (IOException e) {
                    e.printStackTrace();
                } finally {
                    try {
                        bufferedReader.close();
                        bufferedWriter.close();
                        socket.close();
                        socket = null;
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
                flag = false;
                flag_SRX = false;
                button.setText("连接网络");
            }
            }
        });
        mLedON.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (socket == null) return;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            bufferedWriter.append("1\r\n");
                        } catch (IOException e) {
                            e.printStackTrace();
                        } finally {
                            if (bufferedWriter != null) {
                                try {
                                    bufferedWriter.flush();
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                        Log.i("开灯", "TCP上");
                    }
                }).start();
            }
        });
        mLedOFF.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (socket == null) return;
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            bufferedWriter.append("0\r\n");
                        } catch (IOException e) {
                            e.printStackTrace();
                        } finally {
                            if (bufferedWriter != null) {
                                try {
                                    bufferedWriter.flush();
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                            }
                        }
                        Log.i("关灯", "TCP上");
                    }
                }).start();
            }
        });
    }
}
