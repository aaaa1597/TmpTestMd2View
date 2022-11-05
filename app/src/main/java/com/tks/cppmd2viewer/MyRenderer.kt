package com.tks.cppmd2viewer

import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import android.util.Log
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MyRenderer : GLSurfaceView.Renderer {
    // Native functions
    external fun setScreenSize(width: Int, height: Int)
    external fun setRotationAngles(x: Float, y: Float)
    external fun setScale(scale: Float)

    override fun onDrawFrame(gl: GL10?) {
        Jni.onDrawFrame()
    }

    override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
        Log.d("aaaaa", "onSurfaceChanged() width=" + width + "height=" + height)
        gl?.glViewport(0, 0, width, height)
    }

    override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
        Jni.onSurfaceCreated()
    }

    companion object {
        init {
            System.loadLibrary("MD2Loader")
        }
    }
}
