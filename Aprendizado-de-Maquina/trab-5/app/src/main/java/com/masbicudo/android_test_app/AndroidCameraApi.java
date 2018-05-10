package com.masbicudo.android_test_app;

import android.Manifest;
import android.content.Context;
import android.content.pm.ActivityInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.ImageFormat;
import android.graphics.PixelFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CameraMetadata;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.TotalCaptureResult;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Bundle;
import android.os.Debug;
import android.os.Environment;
import android.os.Handler;
import android.os.HandlerThread;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.util.Size;
import android.util.SparseIntArray;
import android.view.Surface;
import android.view.TextureView;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Rect;
import org.opencv.imgcodecs.Imgcodecs;
import org.opencv.imgproc.Imgproc;
import org.opencv.ml.KNearest;
import org.opencv.ml.Ml;
import org.opencv.utils.Converters;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
public class AndroidCameraApi extends AppCompatActivity {
    private static final String TAG = "AndroidCameraApi";
    private TextView tv;
    private ImageView iv;
    private TextureView textureView;
    private static final SparseIntArray ORIENTATIONS = new SparseIntArray();
    static {
        ORIENTATIONS.append(Surface.ROTATION_0, 90);
        ORIENTATIONS.append(Surface.ROTATION_90, 0);
        ORIENTATIONS.append(Surface.ROTATION_180, 270);
        ORIENTATIONS.append(Surface.ROTATION_270, 180);
    }
    private String cameraId;
    protected CameraDevice cameraDevice;
    protected CameraCaptureSession cameraCaptureSessions;
    protected CaptureRequest captureRequest;
    protected CaptureRequest.Builder captureRequestBuilder;
    private Size imageDimension;
    private ImageReader imageReader;
    private File file;
    private static final int REQUEST_CAMERA_PERMISSION = 200;
    private boolean mFlashSupported;
    private Handler mBackgroundHandler;
    private HandlerThread mBackgroundThread;
    private boolean processing = false;
    private KNearest knn;
    private AssetManager assetManager;
    private int frames = 0;
    private Image image = null;
    private android.graphics.Rect zoom = null;
    private boolean mOpenCvLoaded = false;

    static {
        if (!OpenCVLoader.initDebug()) {
            Log.e("TEST", "Cannot connect to OpenCV Manager");
        }
    }

    private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
        @Override
        public void onManagerConnected(int status) {
            switch (status) {
                case LoaderCallbackInterface.SUCCESS:
                {
                    Log.i(TAG, "OpenCV loaded successfully");
                    mOpenCvLoaded = true;

                    try {
                        setupKnn();
                    } catch (IOException e) {
                        e.printStackTrace();

                    }
                } break;
                default:
                {
                    super.onManagerConnected(status);
                } break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_android_camera_api);
        textureView = (TextureView) findViewById(R.id.texture);
        assert textureView != null;
        textureView.setSurfaceTextureListener(textureListener);
        tv = (TextView) findViewById(R.id.txt_tv);
        iv = (ImageView) findViewById(R.id.img_iv);
        assetManager = getAssets();
    }

    private void setupKnn() throws IOException {

        // References:
        //http://answers.opencv.org/question/89927/how-to-use-knearest-in-java/

        InputStream is = assetManager.open("digits.png");
        Bitmap bitmap = BitmapFactory.decodeStream(is);
        Mat digits = new Mat();
        Utils.bitmapToMat(bitmap, digits);

        Mat trainData = new Mat();
        List<Integer> trainLabs = new ArrayList<Integer>();

        for (int r=0; r<50; r++) {
            // 100 digits per row:
            for (int c=0; c<100; c++) {
                // crop out 1 digit:
                Mat instance = digits.submat(new Rect(c*20,r*20,20,20));
                List<Mat> rgb = new ArrayList<Mat>(3);
                Core.split(instance, rgb);
                Mat mR = rgb.get(0);
                Mat mG = rgb.get(1);
                Mat mB = rgb.get(2);

                // we need float data for knn:
                mR.convertTo(mR, CvType.CV_32F);
                // for opencv ml, each instance has to be a single row:
                mR = mR.reshape(1,1);
                trainData.push_back(mR);
                // add a label for that item (the digit number):
                trainLabs.add(r/5);
            }
        }

        knn = KNearest.create();
        knn.train(trainData, Ml.ROW_SAMPLE, Converters.vector_int_to_Mat(trainLabs));
    }

    TextureView.SurfaceTextureListener textureListener = new TextureView.SurfaceTextureListener() {
        @Override
        public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
            //open your camera here
            openCamera();
        }
        @Override
        public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
            // Transform you image captured size according to the surface width and height
        }
        @Override
        public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
            return false;
        }
        @Override
        public void onSurfaceTextureUpdated(SurfaceTexture surface) {
        }
    };
    private final CameraDevice.StateCallback stateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(CameraDevice camera) {
            //This is called when the camera is open
            Log.e(TAG, "onOpened");
            cameraDevice = camera;
            createCameraPreview();
        }
        @Override
        public void onDisconnected(CameraDevice camera) {
            cameraDevice.close();
        }
        @Override
        public void onError(CameraDevice camera, int error) {
            cameraDevice.close();
            cameraDevice = null;
        }
    };
    final CameraCaptureSession.CaptureCallback captureCallbackListener = new CameraCaptureSession.CaptureCallback() {
        @Override
        public void onCaptureCompleted(CameraCaptureSession session, CaptureRequest request, TotalCaptureResult result) {
            super.onCaptureCompleted(session, request, result);
            Toast.makeText(AndroidCameraApi.this, "Saved:" + file, Toast.LENGTH_SHORT).show();
            createCameraPreview();
        }
    };
    protected void startBackgroundThread() {
        mBackgroundThread = new HandlerThread("Camera Background");
        mBackgroundThread.start();
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
    }
    protected void stopBackgroundThread() {
        mBackgroundThread.quitSafely();
        try {
            mBackgroundThread.join();
            mBackgroundThread = null;
            mBackgroundHandler = null;
        } catch (InterruptedException e) {
            e.printStackTrace();
        }
    }
    protected void takePicture() {
        if(null == cameraDevice) {
            Log.e(TAG, "cameraDevice is null");
            return;
        }
        CameraManager manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
        try {
            CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraDevice.getId());
            Size[] jpegSizes = null;
            if (characteristics != null) {
                jpegSizes = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP)
                        .getOutputSizes(ImageFormat.JPEG);
            }
            int width = 640;
            int height = 480;
            if (jpegSizes != null && 0 < jpegSizes.length) {
                width = jpegSizes[0].getWidth();
                height = jpegSizes[0].getHeight();
            }
            ImageReader reader = ImageReader.newInstance(width, height, ImageFormat.JPEG, 1);
            List<Surface> outputSurfaces = new ArrayList<Surface>(2);
            outputSurfaces.add(reader.getSurface());
            outputSurfaces.add(new Surface(textureView.getSurfaceTexture()));
            final CaptureRequest.Builder captureBuilder
                    = cameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_STILL_CAPTURE);
            captureBuilder.addTarget(reader.getSurface());
            captureBuilder.set(CaptureRequest.CONTROL_MODE, CameraMetadata.CONTROL_MODE_AUTO);
            // Orientation
            int rotation = getWindowManager().getDefaultDisplay().getRotation();
            captureBuilder.set(CaptureRequest.JPEG_ORIENTATION, ORIENTATIONS.get(rotation));
            final File file = new File(Environment.getExternalStorageDirectory()+"/pic.jpg");
            ImageReader.OnImageAvailableListener readerListener = new ImageReader.OnImageAvailableListener() {
                @Override
                public void onImageAvailable(ImageReader reader) {
                    Image image = null;
                    try {
                        image = reader.acquireLatestImage();
                        ByteBuffer buffer = image.getPlanes()[0].getBuffer();
                        byte[] bytes = new byte[buffer.capacity()];
                        buffer.get(bytes);
                        save(bytes);
                    } catch (FileNotFoundException e) {
                        e.printStackTrace();
                    } catch (IOException e) {
                        e.printStackTrace();
                    } finally {
                        if (image != null) {
                            image.close();
                        }
                    }
                }
                private void save(byte[] bytes) throws IOException {
                    OutputStream output = null;
                    try {
                        output = new FileOutputStream(file);
                        output.write(bytes);
                    } finally {
                        if (null != output) {
                            output.close();
                        }
                    }
                }
            };
            reader.setOnImageAvailableListener(readerListener, mBackgroundHandler);
            final CameraCaptureSession.CaptureCallback captureListener = new CameraCaptureSession.CaptureCallback() {
                @Override
                public void onCaptureCompleted(CameraCaptureSession session, CaptureRequest request, TotalCaptureResult result) {
                    super.onCaptureCompleted(session, request, result);
                    Toast.makeText(AndroidCameraApi.this, "Saved:" + file, Toast.LENGTH_SHORT).show();
                    createCameraPreview();
                }
            };
            cameraDevice.createCaptureSession(outputSurfaces, new CameraCaptureSession.StateCallback() {
                @Override
                public void onConfigured(CameraCaptureSession session) {
                    try {
                        session.capture(captureBuilder.build(), captureListener, mBackgroundHandler);
                    } catch (CameraAccessException e) {
                        e.printStackTrace();
                    }
                }
                @Override
                public void onConfigureFailed(CameraCaptureSession session) {
                }
            }, mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }
    protected ImageReader getClassifierImageReader() {

        // References
        //https://www.youtube.com/watch?v=Oq3oiCfSgbo
        //https://github.com/caffe2/AICamera/blob/master/app/src/main/java/facebook/f8demo/ClassifyCamera.java
        // This site contains this snippet of code, used to process frames from the camera preview

        int width = 227;
        int height = 227;
        //ImageReader reader = ImageReader.newInstance(width, height, PixelFormat.RGBA_8888, 1);
        ImageReader reader = ImageReader.newInstance(width, height, ImageFormat.YUV_420_888, 4);
        ImageReader.OnImageAvailableListener readerListener = new ImageReader.OnImageAvailableListener() {
            @Override
            public void onImageAvailable(ImageReader reader) {

                // PROBLEM: when I tried to use the bitmap as a local variable,
                // the program crashed after some time time with the message:
                //  "BufferQueue has been abandoned!"
                // This must have something to do with the way that Java handles garbage collection.
                // So I had to store the variable at the instance level of this class.
                //
                //Image image = null;

                try {

                    image = reader.acquireNextImage();
                    if (processing)
                    {
                        image.close();
                        return;
                    }
                    processing = true;

                    frames++;

                    // References:
                    //http://answers.opencv.org/question/52722/what-is-the-correct-way-to-convert-a-mat-to-a-bitmap/
                    //https://www.learnopencv.com/handwritten-digits-classification-an-opencv-c-python-tutorial/
                    //https://www.learnopencv.com/histogram-of-oriented-gradients/

                    final int w = image.getWidth();
                    final int h = image.getHeight();
                    Image.Plane[] planes = image.getPlanes();
//                    ByteBuffer Ybuffer = planes[0].getBuffer();
//                    ByteBuffer Ubuffer = planes[1].getBuffer();
//                    ByteBuffer Vbuffer = planes[2].getBuffer();
//                    // TODO: use these for proper image processing on different formats.
//                    int rowStride = image.getPlanes()[1].getRowStride();
//                    int pixelStride = image.getPlanes()[1].getPixelStride();
//                    byte[] Y = new byte[Ybuffer.capacity()];
//                    byte[] U = new byte[Ubuffer.capacity()];
//                    byte[] V = new byte[Vbuffer.capacity()];
//                    Ybuffer.get(Y);
//                    Ubuffer.get(U);
//                    Vbuffer.get(V);

                    final ByteBuffer buffer = planes[0].getBuffer();
                    final byte[] bytes = new byte[buffer.remaining()];
                    buffer.get(bytes, 0, bytes.length);


                    //Mat data = new Mat(h, w, CvType.CV_8UC1);
                    //data.put(0, 0, Y);
                    //Imgproc( croppedimage, resizeimage, new Size(20,20) );

                    //Mat data = ImageUtils.imageToMat(image);

                    //Mat res = new Mat();
                    //final float predictedClassNum = knn.findNearest(data, 1, res);
                    //final Bitmap bitmap1 = BitmapFactory.decodeByteArray(Y, 0, Y.length, null);
                    //final Bitmap bitmap1 = Bitmap.createBitmap(data.cols(), data.rows(), Bitmap.Config.ARGB_8888);
                    //Utils.matToBitmap(data, bitmap1);

                    // Creating 20x20 gray scale bitmap to send to the KNN classifier
                    final byte[] argbBytes = new byte[bytes.length * 4];
                    for(int i = 0; i < bytes.length; i++) {
                        int y = i % w;
                        int x = h - i / w - 1;
                        int j = y * h + x;
                        argbBytes[j*4 + 0] = bytes[i];
                        argbBytes[j*4 + 1] = bytes[i];
                        argbBytes[j*4 + 2] = bytes[i];
                        argbBytes[j*4 + 3] = (byte)255;
                    }

                    final Bitmap bitmap1 = Bitmap.createBitmap(h, w, Bitmap.Config.ARGB_8888);
                    bitmap1.copyPixelsFromBuffer(ByteBuffer.wrap(argbBytes));
                    final Bitmap bitmap2 = Bitmap.createScaledBitmap(
                            cropCenterSquare(bitmap1),
                            20,
                            20,
                            false);
                    final Bitmap bitmap3 = Bitmap.createScaledBitmap(
                            bitmap2,
                            500,
                            500,
                            false);

                    Mat gs = new Mat();
                    Utils.bitmapToMat(bitmap2, gs);
                    List<Mat> argb = new ArrayList<Mat>(4);
                    Core.split(gs, argb);
                    gs = argb.get(0);
                    // we need float data for knn:
                    gs.convertTo(gs, CvType.CV_32F);
                    // for opencv ml, each instance has to be a single row:
                    gs = gs.reshape(1,1);
                    Mat res = new Mat();
                    final float p = knn.findNearest(gs, 1, res);

                    // Show info
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            iv.setImageBitmap(bitmap3);
                            tv.setText(""
                                    + "class: " + p + "\n"
                                    + "f: " + frames + "\n"
                                    + "sz: " + w + "x" + h + "=" + h*w + "\n"
                                    + "len: " + bytes.length + "\n"
                                    + "o: " + getResources().getConfiguration().orientation
                            );
                            processing = false;
                        }
                    });

                } finally {
                    if (image != null) {
                        image.close();
                    }
                }
            }
        };
        reader.setOnImageAvailableListener(readerListener, mBackgroundHandler);
        return reader;
    }

    private static Bitmap cropCenterSquare(Bitmap srcBmp) {
        if (srcBmp.getWidth() >= srcBmp.getHeight()){
            return Bitmap.createBitmap(
                    srcBmp,
                    srcBmp.getWidth()/2 - srcBmp.getHeight()/2,
                    0,
                    srcBmp.getHeight(),
                    srcBmp.getHeight()
            );
        }else{
            return Bitmap.createBitmap(
                    srcBmp,
                    0,
                    srcBmp.getHeight()/2 - srcBmp.getWidth()/2,
                    srcBmp.getWidth(),
                    srcBmp.getWidth()
            );
        }
    }

    protected void createCameraPreview() {
        try {
            // Activity widget surface: to display the camera image in the screen
            SurfaceTexture texture = textureView.getSurfaceTexture();
            assert texture != null;
            texture.setDefaultBufferSize(imageDimension.getWidth(), imageDimension.getHeight());
            Surface surface = new Surface(texture);
            captureRequestBuilder = cameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            captureRequestBuilder.set(CaptureRequest.SCALER_CROP_REGION, zoom);
            captureRequestBuilder.addTarget(surface);

            // Classifier surface: to allow the classifier to do it's work with the image from the camera
            ImageReader classifierImageReader = getClassifierImageReader();
            captureRequestBuilder.addTarget(classifierImageReader.getSurface());

            cameraDevice.createCaptureSession(
                    Arrays.asList(surface, classifierImageReader.getSurface()),
                    new CameraCaptureSession.StateCallback(){
                @Override
                public void onConfigured(@NonNull CameraCaptureSession cameraCaptureSession) {
                    //The camera is already closed
                    if (null == cameraDevice) {
                        return;
                    }
                    // When the session is ready, we start displaying the preview.
                    cameraCaptureSessions = cameraCaptureSession;
                    updatePreview();
                }
                @Override
                public void onConfigureFailed(@NonNull CameraCaptureSession cameraCaptureSession) {
                    Toast.makeText(AndroidCameraApi.this, "Configuration change", Toast.LENGTH_SHORT).show();
                }
            }, null);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }
    private void openCamera() {
        CameraManager manager = (CameraManager) getSystemService(Context.CAMERA_SERVICE);
        Log.e(TAG, "is camera open");
        try {
            cameraId = manager.getCameraIdList()[0];
            CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraId);
            StreamConfigurationMap map = characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
            assert map != null;
            imageDimension = map.getOutputSizes(SurfaceTexture.class)[0];
            // Add permission for camera and let user grant the permission
            if (ActivityCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED
                    && ActivityCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(AndroidCameraApi.this, new String[]{Manifest.permission.CAMERA, Manifest.permission.WRITE_EXTERNAL_STORAGE}, REQUEST_CAMERA_PERMISSION);
                return;
            }

            android.graphics.Rect rect = characteristics.get(CameraCharacteristics.SENSOR_INFO_ACTIVE_ARRAY_SIZE);
            float zoomLevel = 4;
            float ratio = (float) 1 / zoomLevel;
            int croppedWidth = rect.width() - Math.round((float)rect.width() * ratio);
            int croppedHeight = rect.height() - Math.round((float)rect.height() * ratio);
            zoom = new android.graphics.Rect(croppedWidth/2, croppedHeight/2,
                    rect.width() - croppedWidth/2, rect.height() - croppedHeight/2);

            manager.openCamera(cameraId, stateCallback, null);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
        Log.e(TAG, "openCamera X");
    }
    protected void updatePreview() {
        if(null == cameraDevice) {
            Log.e(TAG, "updatePreview error, return");
        }
        captureRequestBuilder.set(CaptureRequest.CONTROL_MODE, CameraMetadata.CONTROL_MODE_AUTO);
        try {
            cameraCaptureSessions.setRepeatingRequest(captureRequestBuilder.build(), null, mBackgroundHandler);
        } catch (CameraAccessException e) {
            e.printStackTrace();
        }
    }
    private void closeCamera() {
        if (null != cameraDevice) {
            cameraDevice.close();
            cameraDevice = null;
        }
        if (null != imageReader) {
            imageReader.close();
            imageReader = null;
        }
    }
    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if (requestCode == REQUEST_CAMERA_PERMISSION) {
            if (grantResults[0] == PackageManager.PERMISSION_DENIED) {
                // close the app
                Toast.makeText(AndroidCameraApi.this, "Sorry!!!, you can't use this app without granting permission", Toast.LENGTH_LONG).show();
                finish();
            }
        }
    }
    @Override
    protected void onResume() {
        super.onResume();

        OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION_2_4_6, this, mLoaderCallback);

        if (BuildConfig.DEBUG) { // don't even consider it otherwise
            if (Debug.isDebuggerConnected()) {
                Log.d("SCREEN", "Keeping screen on for debugging, detach debugger and force an onResume to turn it off.");
                getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
            } else {
                getWindow().clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                Log.d("SCREEN", "Keeping screen on for debugging is now deactivated.");
            }
        }

        Log.e(TAG, "onResume");
        startBackgroundThread();
        if (textureView.isAvailable()) {
            openCamera();
        } else {
            textureView.setSurfaceTextureListener(textureListener);
        }
    }
    @Override
    protected void onPause() {
        Log.e(TAG, "onPause");
        //closeCamera();
        stopBackgroundThread();
        super.onPause();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_PORTRAIT);
    }
}
