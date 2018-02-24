// Copyright (C) 2018 Kyaw Kyaw Htike @ Ali Abdul Ghafur. All rights reserved.

#include "KKH_stb_image_JNI_wrap.h"


/*
* Class:     KKH_stb_image_JNI_wrap_STBimage
* Method:    imread_jni
* Signature: (Ljava/lang/String;Z)LKKH/StdLib/Matk;
*/
JNIEXPORT jobject JNICALL Java_KKH_stb_1image_1JNI_1wrap_STBimage_imread_1jni
(JNIEnv *env, jclass cls, jstring fpath_, jboolean divBy255_)
{
	jni_utils ju(env);
	std::string fpath = ju.from_jstring(fpath_);
	bool divBy255 = ju.from_jboolean(divBy255_);
	
	int width, height, nchannels;
	unsigned char *data = stbi_load(fpath.c_str(), &width, &height, &nchannels, 0);

	Matk m;
	m.create<unsigned char>(env, data, height, width, nchannels, false, divBy255);

	return m.get_obj();
}

/*
* Class:     KKH_stb_image_JNI_wrap_STBimage
* Method:    imwrite_jni
* Signature: (Ljava/lang/String;LKKH/StdLib/Matk;II)Z
*/
JNIEXPORT jboolean JNICALL Java_KKH_stb_1image_1JNI_1wrap_STBimage_imwrite_1jni
(JNIEnv * env, jclass cls, jstring fpath_, jobject img_, jint image_type_, jint quality_)
{
	jni_utils ju(env);
	std::string fpath = ju.from_jstring(fpath_);
	int quality = ju.from_jint(quality_);
	int image_type = ju.from_jint(image_type_);
	Matk img; img.create(env, img_);

	int width = img.ncols();
	int height = img.nrows();
	int nchannels = img.nchannels();

	std::vector<unsigned char> vecIn = img.to_stdVec<unsigned char>(false, true);
	unsigned char* data_ptr = vecIn.data();

	int status_write;

	switch (image_type)
	{
		case 0:
			printf("JNI: Writing png image file.\n");
			status_write = stbi_write_png(fpath.c_str(), width, height, nchannels, data_ptr, 0);
			break;
		case 1:
			printf("JNI: Writing bmp image file.\n");
			status_write = stbi_write_bmp(fpath.c_str(), width, height, nchannels, data_ptr);
			break;
		case 2:
			printf("JNI: Writing tga image file.\n");
			status_write = stbi_write_tga(fpath.c_str(), width, height, nchannels, data_ptr);
			break;
		case 3:
		{
			printf("JNI: Writing hdr image file.\n");
			std::vector<float> vecIn_float = img.to_stdVec<float>(false, true);
			float* data_ptr_float = vecIn_float.data();
			status_write = stbi_write_hdr(fpath.c_str(), width, height, nchannels, data_ptr_float);
			break;
		}			
		case 4:
			printf("JNI: Writing jpg image file.\n");
			status_write = stbi_write_jpg(fpath.c_str(), width, height, nchannels, data_ptr, quality);
			break;
	}

	return ju.to_jboolean(status_write != 0); 
	
}

/*
* Class:     KKH_stb_image_JNI_wrap_STBimage
* Method:    imresize_jni
* Signature: (LKKH/StdLib/Matk;II)LKKH/StdLib/Matk;
*/
JNIEXPORT jobject JNICALL Java_KKH_stb_1image_1JNI_1wrap_STBimage_imresize_1jni
(JNIEnv *env , jclass cls, jobject img_, jint width_new_, jint height_new_)
{
	jni_utils ju(env);

	Matk img; 
	img.create(env, img_);
	int width = img.ncols();
	int height = img.nrows();
	int nchannels = img.nchannels();
	int width_new = ju.from_jint(width_new_);
	int height_new = ju.from_jint(height_new_);
	
	std::vector<unsigned char> vecIn = img.to_stdVec<unsigned char>(false, true);
	unsigned char* data_ptr_in = vecIn.data();
	std::vector<unsigned char> vecOut(width_new * height_new * nchannels);
	unsigned char* data_ptr_out = vecOut.data();

	int input_stride_in_bytes = 0; // 0 means continuously packed memory
	int output_stride_in_bytes = 0; // 0 means continuously packed memory
			
	// do the actual resizing
	stbir_resize_uint8(data_ptr_in, width, height, input_stride_in_bytes, data_ptr_out, width_new, height_new, output_stride_in_bytes, nchannels);
	
	Matk img_out;
	img_out.create<unsigned char>(env, data_ptr_out, height_new, width_new, nchannels, false, true);

	return img_out.get_obj();

}