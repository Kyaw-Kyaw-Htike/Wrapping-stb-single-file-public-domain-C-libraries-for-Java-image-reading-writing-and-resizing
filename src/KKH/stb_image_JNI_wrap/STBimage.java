/**
 * Copyright (C) 2018 Kyaw Kyaw Htike @ Ali Abdul Ghafur. All rights reserved.
 *
 */

package KKH.stb_image_JNI_wrap;

import KKH.StdLib.Matk;
import KKH.StdLib.stdfuncs;
import KKH.StdLib.Matk.ImageType;

public final class STBimage {	
	
	static
	{
		System.loadLibrary("KKH_stb_image_JNI_wrap");
	}
	
	// image type for writing image to disk
    public static enum ImageType
    { 
    	PNG,
    	BMP,
    	TGA,
    	HDR,
    	JPG;
	}
    
	private static Matk prepare_img_before_jni(Matk img)
	{
		Matk img_prepped; 
			
    	if(!img.is_image())
    		throw new IllegalArgumentException("ERROR: the input matrix is not an image. Therefore, cannot convert proceed.");
    	
    	if(img.check_if_image_range_0_255())
    		img_prepped = img.div(255);
    	else    		
    		img_prepped = img.copy_deep();
			
		switch(img.get_image_type())
		{
			case GRAY:
			case RGB:
			case RGBA:
				break;
			case BGR:
				img_prepped = img_prepped.get_channels(new int[] {2,1,0});
				break;
			case BGRA:
				img_prepped = img_prepped.get_channels(new int[] {2,1,0,3});
				break;
			case ABGR:
				img_prepped = img_prepped.get_channels(new int[] {3,2,1,0});
				break;
			case ARGB:
				img_prepped = img_prepped.get_channels(new int[] {1,2,3,0});
				break;
			default:
				throw new IllegalArgumentException("ERROR: Failed preparing image for JNI. Input image has a type that is not valid for further processing.");
		}
		
		return img_prepped;
	}
	
	private static void prepare_img_after_jni(Matk img)
	{
		int nchannels = img.nchannels();
		Matk.ImageType img_type;
		switch(nchannels)
		{
			case 1:
				img_type = Matk.ImageType.GRAY;
				break;
			case 3:
				img_type = Matk.ImageType.RGB;
				break;
			case 4:
				img_type = Matk.ImageType.RGBA;
				break;
			default:
				throw new IllegalArgumentException("ERROR: STBimage JNI returns an image that has number of channels not equal to either 1, 3 or 4. So cannot determine the image type.");
		}
		img.set_as_image(img_type, false);
	}
    
	public static Matk imread(String fpath)
	{		
		Matk img = imread_jni(fpath, true);
		prepare_img_after_jni(img);
		return img;
	}
	
	private static native Matk imread_jni(String fpath, boolean divBy255);
	
	// returns true on success of write. false otherwise
	public static boolean imwrite(String fpath, Matk img, ImageType img_type)
	{
		return imwrite_jni(fpath, prepare_img_before_jni(img), img_type.ordinal(), 100);
	}	
		
	
	// returns true on success of write. false otherwise
	// auto determine image type by looking at file extension
	public static boolean imwrite(String fpath, Matk img)
	{
		String file_extension = stdfuncs.get_file_extension(fpath);
		file_extension = file_extension.toLowerCase();
		ImageType img_type;
		switch(file_extension)
		{
			case "jpg":
			case "jpeg":
			case "jpe":
			case "jfif":
			case "jif":
				img_type = ImageType.JPG;
				break;
			case "png":
				img_type = ImageType.PNG;
				break;
			case "tga":
				img_type = ImageType.TGA;
				break;
			case "hdr":
				img_type = ImageType.HDR;
				break;	
			default:
				throw new IllegalArgumentException("ERROR: STBimage cannot write image with " + file_extension + " extension.");
		}
		return imwrite_jni(fpath, prepare_img_before_jni(img), img_type.ordinal(), 100);
	}	
	
	// note: quality only applies in case of jpg images should be between 1 to 100.
	private static native boolean imwrite_jni(String fpath, Matk img, int image_type, int quality);
	
	public static Matk imresize(Matk img, double scale)
	{		
		int width_new = (int)Math.round((double)img.ncols() * scale); 
		int height_new = (int)Math.round((double)img.nrows() * scale); 	
		return imresize(img, width_new, height_new); 
	}
	
	public static Matk imresize(Matk img, int width_new, int height_new)
	{		
		Matk img_resized = imresize_jni(prepare_img_before_jni(img), width_new, height_new);
		prepare_img_after_jni(img_resized);
		return img_resized;
	}
	
	private static native Matk imresize_jni(Matk img, int width_new, int height_new); 
	
}
