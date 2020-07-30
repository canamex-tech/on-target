/*
The MIT License

Copyright (c) 2019-Present, ROBERT HOWELL

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in-
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef _DSL_DISPLAY_TYPES_H
#define _DSL_DISPLAY_TYPES_H

#include "Dsl.h"
#include "DslApi.h"
#include "DslBase.h"

#define MAX_DISPLAY_LEN 64

namespace DSL
{
    #define DSL_DISPLAY_TYPE_PTR std::shared_ptr<DisplayType>

    #define DSL_RGBA_COLOR_PTR std::shared_ptr<RgbaColor>
    #define DSL_RGBA_COLOR_NEW(name, red, green, blue, alpha) \
        std::shared_ptr<RgbaColor>(new RgbaColor(name, red, green, blue, alpha))

    #define DSL_RGBA_FONT_PTR std::shared_ptr<RgbaFont>
    #define DSL_RGBA_FONT_NEW(name, font, size, pColor) \
        std::shared_ptr<RgbaFont>(new RgbaFont(name, font, size, pColor))

    #define DSL_RGBA_TEXT_PTR std::shared_ptr<RgbaText>
    #define DSL_RGBA_TEXT_NEW(name, text, x_offset, y_offset, font, hasBgColor, pBgColor) \
        std::shared_ptr<RgbaText>(new RgbaText(name, text, x_offset, y_offset, font, hasBgColor, pBgColor))
        
    #define DSL_RGBA_LINE_PTR std::shared_ptr<RgbaLine>
    #define DSL_RGBA_LINE_NEW(name, x1, y1, x2, y2, width, pColor) \
        std::shared_ptr<RgbaLine>(new RgbaLine(name, x1, y1, x2, y2, width, pColor))

    #define DSL_RGBA_ARROW_PTR std::shared_ptr<RgbaArrow>
    #define DSL_RGBA_ARROW_NEW(name, x1, y1, x2, y2, width, head, pColor) \
        std::shared_ptr<RgbaArrow>(new RgbaArrow(name, x1, y1, x2, y2, width, head, pColor))

    #define DSL_RGBA_RECTANGLE_PTR std::shared_ptr<RgbaRectangle>
    #define DSL_RGBA_RECTANGLE_NEW(name, left, top, width, height, borderWidth, pColor, hasBgColor, pBgColor) \
        std::shared_ptr<RgbaRectangle>(new RgbaRectangle(name, left, top, width, height, borderWidth, pColor, hasBgColor, pBgColor))

    #define DSL_RGBA_CIRCLE_PTR std::shared_ptr<RgbaCircle>
    #define DSL_RGBA_CIRCLE_NEW(name, x_center, y_center, radius, pColor, hasBgColor, pBgColor) \
        std::shared_ptr<RgbaCircle>(new RgbaCircle(name, x_center, y_center, radius, pColor, hasBgColor, pBgColor))

    #define DSL_SOURCE_DIMENSIONS_PTR std::shared_ptr<SourceDimensions>
    #define DSL_SOURCE_DIMENSIONS_NEW(name, x_offset, y_offset, font, hasBgColor, pBgColor) \
        std::shared_ptr<SourceDimensions>(new SourceDimensions(name, x_offset, y_offset, font, hasBgColor, pBgColor))

    #define DSL_SOURCE_FRAME_RATE_PTR std::shared_ptr<SourceFrameRate>
    #define DSL_SOURCE_FRAME_RATE_NEW(name, x_offset, y_offset, font, hasBgColor, pBgColor) \
        std::shared_ptr<SourceFrameRate>(new SourceFrameRate(name, x_offset, y_offset, font, hasBgColor, pBgColor))

    #define DSL_SOURCE_NAME_PTR std::shared_ptr<SourceName>
    #define DSL_SOURCE_NAME_NEW(name, x_offset, y_offset, font, hasBgColor, pBgColor) \
        std::shared_ptr<SourceName>(new SourceName(name, x_offset, y_offset, font, hasBgColor, pBgColor))

    // ********************************************************************

    class DisplayType : public Base
    {
    public: 
    
        /**
         * @brief ctor for the virtual DisplayType
         * @param[in] name unique name for the RGBA Color
         */
        DisplayType(const char* name);

        ~DisplayType();
        
        virtual void OverlayFrame(NvDsBatchMeta* pBatchMeta, NvDsFrameMeta* pFrameMeta);
    };
    
    // ********************************************************************

    class RgbaColor : public DisplayType, public NvOSD_ColorParams
    {
    public: 
    
        /**
         * @brief ctor for RGBA Color
         * @param[in] name unique name for the RGBA Color
         * @param[in] red red level for the RGB color [0..1]
         * @param[in] blue blue level for the RGB color [0..1]
         * @param[in] green green level for the RGB color [0..1]
         * @param[in] alpha alpha level for the RGB color [0..1]
         */
        RgbaColor(const char* name, double red, double green, double blue, double alpha);

        ~RgbaColor();
    };
    
    // ********************************************************************

    class RgbaFont : public DisplayType, public NvOSD_FontParams
    {
    public:
    
        /**
         * @brief ctor for RGBA Font
         * @param[in] name standard string name of the actual font type
         * @param[in] size size of the font
         * @param[in] color RGBA Color for the RGBA font
         */
        RgbaFont(const char* name, const char* font, uint size, DSL_RGBA_COLOR_PTR color);

        ~RgbaFont();
        
        std::string m_fontName;
    };
    
    // ********************************************************************

    class RgbaText : public DisplayType, public NvOSD_TextParams
    {
    public:

        /**
         * @brief ctor for RGBA Text
         * @param[in] name unique name of the RGBA Text
         * @param[in] text text string to display
         * @param[in] x_offset starting x positional offset
         * @param[in] y_offset starting y positional offset
         * @param[in] font RGBA font to use for the display dext
         * @param[in] hasBgColor set to true to enable bacground color, false otherwise
         * @param[in] pBgColor RGBA Color for the Text background if set
         */
        RgbaText(const char* name, const char* text, uint x_offset, uint y_offset, 
            DSL_RGBA_FONT_PTR pFont, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor);

        ~RgbaText();

        void OverlayFrame(NvDsBatchMeta* pBatchMeta, NvDsFrameMeta* pFrameMeta);
        
        std::string m_text;
        
        DSL_RGBA_FONT_PTR m_pFont;
    
    };
    
    // ********************************************************************

    class RgbaLine : public DisplayType, public NvOSD_LineParams
    {
    public:

        /**
         * @brief ctor for RGBA Line
         * @param[in] name unique name for the RGBA LIne
         * @param[in] x1 starting x positional offest
         * @param[in] y1 starting y positional offest
         * @param[in] x2 ending x positional offest
         * @param[in] y2 ending y positional offest
         * @param[in] width width of the line in pixels
         * @param[in] color RGBA Color for thIS RGBA Line
         */
        RgbaLine(const char* name, uint x1, uint y1, uint x2, uint y2, 
            uint width, DSL_RGBA_COLOR_PTR pColor);

        ~RgbaLine();

        void OverlayFrame(NvDsBatchMeta* pBatchMeta, NvDsFrameMeta* pFrameMeta);
    };
    
    // ********************************************************************

    class RgbaArrow : public DisplayType, public NvOSD_ArrowParams
    {
    public:

        /**
         * @brief ctor for RGBA Line
         * @param[in] name unique name for the RGBA Arrow
         * @param[in] x1 starting x positional offest
         * @param[in] y1 starting y positional offest
         * @param[in] x2 ending x positional offest
         * @param[in] y2 ending y positional offest
         * @param[in] width width of the line in pixels
         * @param[in] head position of arrow head START_HEAD, END_HEAD, BOTH_HEAD
         * @param[in] pColor RGBA Color for thIS RGBA Line
         */
        RgbaArrow(const char* name, uint x1, uint y1, uint x2, uint y2, 
            uint width, uint head, DSL_RGBA_COLOR_PTR pColor);

        ~RgbaArrow();

        void OverlayFrame(NvDsBatchMeta* pBatchMeta, NvDsFrameMeta* pFrameMeta);
    };

    // ********************************************************************

    class RgbaRectangle : public DisplayType, public NvOSD_RectParams
    {
    public:

        /**
         * @brief ctor for RGBA Rectangle
         * @param[in] name unique name for the RGBA Rectangle
         * @param[in] left left positional offest
         * @param[in] top positional offest
         * @param[in] width width of the rectangle in Pixels
         * @param[in] height height of the rectangle in Pixels
         * @param[in] width width of the line in pixels
         * @param[in] pColor RGBA Color for thIS RGBA Line
         * @param[in] hasBgColor set to true to enable bacground color, false otherwise
         * @param[in] pBgColor RGBA Color for the Circle background if set
         */
        RgbaRectangle(const char* name, uint left, uint top, uint width, uint height, 
            uint borderWidth, DSL_RGBA_COLOR_PTR pColor, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor);

        ~RgbaRectangle();

        void OverlayFrame(NvDsBatchMeta* pBatchMeta, NvDsFrameMeta* pFrameMeta);
    };
    
    // ********************************************************************

    class RgbaCircle : public DisplayType, public NvOSD_CircleParams
    {
    public:

        /**
         * @brief ctor for RGBA Circle
         * @param[in] name unique name for the RGBA Circle
         * @param[in] x_center X positional offset to center of Circle
         * @param[in] y_center y positional offset to center of Circle
         * @param[in] radius radius of the RGBA Circle in pixels 
         * @param[in] pColor RGBA Color for the RGBA Circle
         * @param[in] hasBgColor set to true to enable bacground color, false otherwise
         * @param[in] pBgColor RGBA Color for the Circle background if set
         */
        RgbaCircle(const char* name, uint x_center, uint y_center, uint radius,
            DSL_RGBA_COLOR_PTR pColor, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor);

        ~RgbaCircle();

        void OverlayFrame(NvDsBatchMeta* pBatchMeta, NvDsFrameMeta* pFrameMeta);
    };

    // ********************************************************************

    class SourceDimensions : public DisplayType, public NvOSD_TextParams
    {
    public:

        /**
         * @brief ctor for RGBA Text
         * @param[in] name unique name of the SourceDimensions Display
         * @param[in] x_offset starting x positional offset
         * @param[in] y_offset starting y positional offset
         * @param[in] font RGBA font to use for the display dext
         * @param[in] hasBgColor set to true to enable bacground color, false otherwise
         * @param[in] pBgColor RGBA Color for the Text background if set
         */
        SourceDimensions(const char* name, uint x_offset, uint y_offset, 
            DSL_RGBA_FONT_PTR pFont, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor);

        ~SourceDimensions();

        void OverlayFrame(NvDsBatchMeta* pBatchMeta, NvDsFrameMeta* pFrameMeta);
        
        DSL_RGBA_FONT_PTR m_pFont;
    };

    // ********************************************************************

    class SourceFrameRate : public DisplayType, public NvOSD_TextParams
    {
    public:

        /**
         * @brief ctor for RGBA Text
         * @param[in] name unique name of the SourceFrameRate Display
         * @param[in] x_offset starting x positional offset
         * @param[in] y_offset starting y positional offset
         * @param[in] font RGBA font to use for the display dext
         * @param[in] hasBgColor set to true to enable bacground color, false otherwise
         * @param[in] pBgColor RGBA Color for the Text background if set
         */
        SourceFrameRate(const char* name, uint x_offset, uint y_offset, 
            DSL_RGBA_FONT_PTR pFont, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor);

        ~SourceFrameRate();

        void OverlayFrame(NvDsBatchMeta* pBatchMeta, NvDsFrameMeta* pFrameMeta);
        
        DSL_RGBA_FONT_PTR m_pFont;
    };

    // ********************************************************************

    class SourceName : public DisplayType, public NvOSD_TextParams
    {
    public:

        /**
         * @brief ctor for RGBA Text
         * @param[in] name unique name of the SourceName Display
         * @param[in] x_offset starting x positional offset
         * @param[in] y_offset starting y positional offset
         * @param[in] font RGBA font to use for the display dext
         * @param[in] hasBgColor set to true to enable bacground color, false otherwise
         * @param[in] pBgColor RGBA Color for the Text background if set
         */
        SourceName(const char* name, uint x_offset, uint y_offset, 
            DSL_RGBA_FONT_PTR pFont, bool hasBgColor, DSL_RGBA_COLOR_PTR pBgColor);

        ~SourceName();

        void OverlayFrame(NvDsBatchMeta* pBatchMeta, NvDsFrameMeta* pFrameMeta);
        
        DSL_RGBA_FONT_PTR m_pFont;
    };

}
#endif // _DSL_DISPLAY_TYPES_H
    
