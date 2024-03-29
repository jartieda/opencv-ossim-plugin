#ifndef ossimOpenCVCannyFilter_HEADER
#define ossimOpenCVCannyFilter_HEADER

#include "ossim/plugin/ossimSharedObjectBridge.h"
#include "ossim/base/ossimString.h"
#include "ossim/imaging/ossimImageSourceFilter.h"
#include <ossim/imaging/ossimImageDataFactory.h>

#include <cv.h>
#include <highgui.h>

/** @brief OpenCV Canny Filter 
 **
 ** Implements Canny algorithm for edge detection.
 ** @param threshold1 The first threshold     
 ** @param threshold2 The second threshold
 ** @param aperture_size Aperture parameter for Sobel operator
 **
 ** The function cvCanny finds the edges on the input image image and marks them in the output image edges using the Canny algorithm.
 ** The smallest of threshold1 and threshold2 is used for edge linking, the largest is used to find initial segments of strong edges. 
 **
 **/
class ossimOpenCVCannyFilter : public ossimImageSourceFilter
{
public:
   ossimOpenCVCannyFilter(ossimObject* owner=NULL);
   ossimOpenCVCannyFilter(ossimImageSource* inputSource, double threshold1, double threshold2, int apertureSize);
   ossimOpenCVCannyFilter(ossimObject* owner,ossimImageSource* inputSource, double threshold1, double threshold2, int apertureSize);
   virtual ~ossimOpenCVCannyFilter();
   ossimString getShortName()const
      {
         return ossimString("OpenCVCanny");
      }
   
   ossimString getLongName()const
      {
         return ossimString("OpenCV Canny Filter");
      }
   
   virtual ossimRefPtr<ossimImageData> getTile(const ossimIrect& tileRect, ossim_uint32 resLevel=0);
   virtual void initialize();
   virtual ossimScalarType getOutputScalarType() const;
   ossim_uint32 getNumberOfOutputBands() const;
   virtual bool saveState(ossimKeywordlist& kwl, const char* prefix=0)const;
   virtual bool loadState(const ossimKeywordlist& kwl, const char* prefix=0);

   /*
   * Methods to expose thresholds for adjustment through the GUI
   */
   virtual void setProperty(ossimRefPtr<ossimProperty> property);
   virtual ossimRefPtr<ossimProperty> getProperty(const ossimString& name)const;
   virtual void getPropertyNames(std::vector<ossimString>& propertyNames)const;

protected:
   ossimRefPtr<ossimImageData> theTile;
   void runUcharTransformation(ossimImageData* tile);
   double theThreshold1;
   double theThreshold2;
   int theApertureSize;

TYPE_DATA
};

#endif
