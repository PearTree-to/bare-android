package com.holepunch.bare;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.webkit.WebSettings;
import android.webkit.WebView;


public class MainActivity extends Activity {
  static {
    System.loadLibrary("bare_android_shared");
  }

  private WebView webView;

  public native void init();

  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    setContentView(R.layout.mainactivity);
    // Initialize WebView
    webView = findViewById(R.id.webView);
    WebSettings webSettings = webView.getSettings();
    webSettings.setJavaScriptEnabled(true); // Enable JavaScript
    webView.loadUrl("file:///android_asset/index.html"); // Load HTML file from assets

    init();
  }
}
