package com.tks.cppmd2viewer

import android.content.res.AssetManager

class Jni {
    companion object {
        init {
            System.loadLibrary("jni")
        }

        external fun onStart(assets: AssetManager?, modelnames: Array<String?>?, md2filenames: Array<String?>?, texfilenames: Array<String?>?, vshfilenames: Array<String?>?, fshfilenames: Array<String?>? ): Boolean
        external fun onSurfaceCreated()
        external fun onSurfaceChanged(w: Int, h: Int)
        external fun onDrawFrame()
        external fun onStop()
        external fun setModelPosition(modelname: String?, x: Float, y: Float, z: Float)
        external fun setScale(mScale: Float)
        external fun setRotate(mTouchAngleX: Float, mTouchAngleY: Float)
    }
}