#include "GUI_Paint.h"
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h> //memset()

const uint8_t num[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

const uint8_t Dx = 14;
const uint8_t Dx_2 = 7;
const uint8_t Dx_4 = 3;

/* reverse:  reverse string s in place */
void reverse(char s[]) {
    int i, j;
    char c;

    for (i = 0, j = strlen(s) - 1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* inttostr:  convert n to characters in s */
uint8_t inttostr(int n, char s[]) {
    int i, sign;

    if ((sign = n) < 0) /* record sign */
        n = -n;         /* make n positive */
    i = 0;
    do {                       /* generate digits in reverse order */
        s[i++] = n % 10 + '0'; /* get next digit */
    } while ((n /= 10) > 0);   /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);
    return i;
}

PAINT Paint;

/******************************************************************************
function: Create Image
parameter:
    image   :   Pointer to the image cache
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
******************************************************************************/
void Paint_NewImage(uint8_t* image, uint16_t Width, uint16_t Height,
                    uint16_t Rotate, uint16_t Color) {
    Paint.Image = image;

    Paint.WidthMemory = Width;
    Paint.HeightMemory = Height;
    Paint.Color = Color;
    Paint.Scale = 2;
    Paint.WidthByte = (Width % 8 == 0) ? (Width / 8) : (Width / 8 + 1);
    Paint.HeightByte = Height;

    Paint.Rotate = Rotate;
    Paint.Mirror = MIRROR_NONE;

    if (Rotate == ROTATE_0 || Rotate == ROTATE_180) {
        Paint.Width = Width;
        Paint.Height = Height;
    } else {
        Paint.Width = Height;
        Paint.Height = Width;
    }
}

/******************************************************************************
function: Select Image
parameter:
    image : Pointer to the image cache
******************************************************************************/
void Paint_SelectImage(uint8_t* image) { Paint.Image = image; }

/******************************************************************************
function: Select Image Rotate
parameter:
    Rotate : 0,90,180,270
******************************************************************************/
void Paint_SetRotate(uint16_t Rotate) {
    if (Rotate == ROTATE_0 || Rotate == ROTATE_90 || Rotate == ROTATE_180 ||
        Rotate == ROTATE_270) {
        // printf("Set image Rotate %d\r\n", Rotate);
        Paint.Rotate = Rotate;
    } else {
        // printf("rotate = 0, 90, 180, 270\r\n");
    }
}

/******************************************************************************
function:	Select Image mirror
parameter:
    mirror   :Not mirror,Horizontal mirror,Vertical mirror,Origin mirror
******************************************************************************/
void Paint_SetMirroring(uint8_t mirror) {
    if (mirror == MIRROR_NONE || mirror == MIRROR_HORIZONTAL ||
        mirror == MIRROR_VERTICAL || mirror == MIRROR_ORIGIN) {
        // printf("mirror image x:%s, y:%s\r\n",(mirror & 0x01)?
        // "mirror":"none", ((mirror >> 1) & 0x01)? "mirror":"none");
        Paint.Mirror = mirror;
    } else {
        // printf("mirror should be MIRROR_NONE, MIRROR_HORIZONTAL, \
        MIRROR_VERTICAL or MIRROR_ORIGIN\r\n");
    }
}

void Paint_SetScale(uint8_t scale) {
    if (scale == 2) {
        Paint.Scale = scale;
        Paint.WidthByte = (Paint.WidthMemory % 8 == 0)
                              ? (Paint.WidthMemory / 8)
                              : (Paint.WidthMemory / 8 + 1);
    } else if (scale == 4) {
        Paint.Scale = scale;
        Paint.WidthByte = (Paint.WidthMemory % 4 == 0)
                              ? (Paint.WidthMemory / 4)
                              : (Paint.WidthMemory / 4 + 1);
    } else {
        // printf("Set Scale Input parameter error\r\n");
        // printf("Scale Only support: 2 4 \r\n");
    }
}
/******************************************************************************
function: Draw Pixels
parameter:
    Xpoint : At point X
    Ypoint : At point Y
    Color  : Painted colors
******************************************************************************/
void Paint_SetPixel(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color) {
    if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
        // printf("Exceeding display boundaries\r\n");
        return;
    }
    uint16_t X, Y;
    switch (Paint.Rotate) {
    case 0:
        X = Xpoint;
        Y = Ypoint;
        break;
    case 90:
        X = Paint.WidthMemory - Ypoint - 1;
        Y = Xpoint;
        break;
    case 180:
        X = Paint.WidthMemory - Xpoint - 1;
        Y = Paint.HeightMemory - Ypoint - 1;
        break;
    case 270:
        X = Ypoint;
        Y = Paint.HeightMemory - Xpoint - 1;
        break;
    default:
        return;
    }

    switch (Paint.Mirror) {
    case MIRROR_NONE:
        break;
    case MIRROR_HORIZONTAL:
        X = Paint.WidthMemory - X - 1;
        break;
    case MIRROR_VERTICAL:
        Y = Paint.HeightMemory - Y - 1;
        break;
    case MIRROR_ORIGIN:
        X = Paint.WidthMemory - X - 1;
        Y = Paint.HeightMemory - Y - 1;
        break;
    default:
        return;
    }

    if (X > Paint.WidthMemory || Y > Paint.HeightMemory) {
        // printf("Exceeding display boundaries\r\n");
        return;
    }

    if (Paint.Scale == 2) {
        uint32_t Addr = X / 8 + Y * Paint.WidthByte;
        uint8_t Rdata = Paint.Image[Addr];
        if (Color == BLACK)
            Paint.Image[Addr] = Rdata & ~(0x80 >> (X % 8));
        else
            Paint.Image[Addr] = Rdata | (0x80 >> (X % 8));
    } else if (Paint.Scale == 4) {
        uint32_t Addr = X / 4 + Y * Paint.WidthByte;
        Color = Color % 4; // Guaranteed color scale is 4  --- 0~3
        uint8_t Rdata = Paint.Image[Addr];

        Rdata = Rdata & (~(0xC0 >> ((X % 4) * 2)));
        Paint.Image[Addr] = Rdata | ((Color << 6) >> ((X % 4) * 2));
    }
}

/******************************************************************************
function: Clear the color of the picture
parameter:
    Color : Painted colors
******************************************************************************/
void Paint_Clear(uint16_t Color) {
    uint16_t X, Y;
    uint32_t Addr;
    for (Y = 0; Y < Paint.HeightByte; Y++) {
        for (X = 0; X < Paint.WidthByte; X++) { // 8 pixel =  1 byte
            Addr = X + Y * Paint.WidthByte;
            Paint.Image[Addr] = Color;
        }
    }
}

/******************************************************************************
function: Clear the color of a window
parameter:
    Xstart : x starting point
    Ystart : Y starting point
    Xend   : x end point
    Yend   : y end point
    Color  : Painted colors
******************************************************************************/
void Paint_ClearWindows(uint16_t Xstart, uint16_t Ystart, uint16_t Xend,
                        uint16_t Yend, uint16_t Color) {
    uint16_t X, Y;
    for (Y = Ystart; Y < Yend; Y++) {
        for (X = Xstart; X < Xend; X++) { // 8 pixel =  1 byte
            Paint_SetPixel(X, Y, Color);
        }
    }
}

/******************************************************************************
function: Draw Point(Xpoint, Ypoint) Fill the color
parameter:
    Xpoint		: The Xpoint coordinate of the point
    Ypoint		: The Ypoint coordinate of the point
    Color		: Painted color
    Dot_Pixel	: point size
    Dot_Style	: point Style
******************************************************************************/
void Paint_DrawPoint(uint16_t Xpoint, uint16_t Ypoint, uint16_t Color,
                     DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_Style) {
    if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
        // printf("Paint_DrawPoint Input exceeds the normal display range\r\n");
        return;
    }

    int16_t XDir_Num, YDir_Num;
    if (Dot_Style == DOT_FILL_AROUND) {
        for (XDir_Num = 0; XDir_Num < 2 * Dot_Pixel - 1; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num < 2 * Dot_Pixel - 1; YDir_Num++) {
                if (Xpoint + XDir_Num - Dot_Pixel < 0 ||
                    Ypoint + YDir_Num - Dot_Pixel < 0)
                    break;
                //                printf("x = %d, y = %d\r\n", Xpoint + XDir_Num
                //                - Dot_Pixel, Ypoint + YDir_Num - Dot_Pixel);
                Paint_SetPixel(Xpoint + XDir_Num - Dot_Pixel,
                               Ypoint + YDir_Num - Dot_Pixel, Color);
            }
        }
    } else {
        for (XDir_Num = 0; XDir_Num < Dot_Pixel; XDir_Num++) {
            for (YDir_Num = 0; YDir_Num < Dot_Pixel; YDir_Num++) {
                Paint_SetPixel(Xpoint + XDir_Num - 1, Ypoint + YDir_Num - 1,
                               Color);
            }
        }
    }
}

/******************************************************************************
function: Draw a line of arbitrary slope
parameter:
    Xstart ：Starting Xpoint point coordinates
    Ystart ：Starting Xpoint point coordinates
    Xend   ：End point Xpoint coordinate
    Yend   ：End point Ypoint coordinate
    Color  ：The color of the line segment
    Line_width : Line width
    Line_Style: Solid and dotted lines
******************************************************************************/
void Paint_DrawLine(uint16_t Xstart, uint16_t Ystart, uint16_t Xend,
                    uint16_t Yend, uint16_t Color, DOT_PIXEL Line_width,
                    LINE_STYLE Line_Style) {
    if (Xstart > Paint.Width || Ystart > Paint.Height || Xend > Paint.Width ||
        Yend > Paint.Height) {
        // printf("Paint_DrawLine Input exceeds the normal display range\r\n");
        return;
    }

    uint16_t Xpoint = Xstart;
    uint16_t Ypoint = Ystart;
    int dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
    int dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

    // Increment direction, 1 is positive, -1 is counter;
    int XAddway = Xstart < Xend ? 1 : -1;
    int YAddway = Ystart < Yend ? 1 : -1;

    // Cumulative error
    int Esp = dx + dy;
    char Dotted_Len = 0;

    for (;;) {
        Dotted_Len++;
        // Painted dotted line, 2 point is really virtual
        if (Line_Style == LINE_STYLE_DOTTED && Dotted_Len % 3 == 0) {
            //            printf("LINE_DOTTED\r\n");
            Paint_DrawPoint(Xpoint, Ypoint, IMAGE_BACKGROUND, Line_width,
                            DOT_STYLE_DFT);
            Dotted_Len = 0;
        } else {
            Paint_DrawPoint(Xpoint, Ypoint, Color, Line_width, DOT_STYLE_DFT);
        }
        if (2 * Esp >= dy) {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx) {
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}

/******************************************************************************
function: Draw a rectangle
parameter:
    Xstart ：Rectangular  Starting Xpoint point coordinates
    Ystart ：Rectangular  Starting Xpoint point coordinates
    Xend   ：Rectangular  End point Xpoint coordinate
    Yend   ：Rectangular  End point Ypoint coordinate
    Color  ：The color of the Rectangular segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the rectangle
******************************************************************************/
void Paint_DrawRectangle(uint16_t Xstart, uint16_t Ystart, uint16_t Xend,
                         uint16_t Yend, uint16_t Color, DOT_PIXEL Line_width,
                         DRAW_FILL Draw_Fill) {
    if (Xstart > Paint.Width || Ystart > Paint.Height || Xend > Paint.Width ||
        Yend > Paint.Height) {
        // printf("Input exceeds the normal display range\r\n");
        return;
    }

    if (Draw_Fill) {
        uint16_t Ypoint;
        for (Ypoint = Ystart; Ypoint < Yend; Ypoint++) {
            Paint_DrawLine(Xstart, Ypoint, Xend, Ypoint, Color, Line_width,
                           LINE_STYLE_SOLID);
        }
    } else {
        Paint_DrawLine(Xstart, Ystart, Xend, Ystart, Color, Line_width,
                       LINE_STYLE_SOLID);
        Paint_DrawLine(Xstart, Ystart, Xstart, Yend, Color, Line_width,
                       LINE_STYLE_SOLID);
        Paint_DrawLine(Xend, Yend, Xend, Ystart, Color, Line_width,
                       LINE_STYLE_SOLID);
        Paint_DrawLine(Xend, Yend, Xstart, Yend, Color, Line_width,
                       LINE_STYLE_SOLID);
    }
}

/******************************************************************************
function: Use the 8-point method to draw a circle of the
            specified size at the specified position->
parameter:
    X_Center  ：Center X coordinate
    Y_Center  ：Center Y coordinate
    Radius    ：circle Radius
    Color     ：The color of the ：circle segment
    Line_width: Line width
    Draw_Fill : Whether to fill the inside of the Circle
******************************************************************************/
void Paint_DrawCircle(uint16_t X_Center, uint16_t Y_Center, uint16_t Radius,
                      uint16_t Color, DOT_PIXEL Line_width,
                      DRAW_FILL Draw_Fill) {
    if (X_Center > Paint.Width || Y_Center >= Paint.Height) {
        // printf("Paint_DrawCircle Input exceeds the normal display
        // range\r\n");
        return;
    }

    // Draw a circle from(0, R) as a starting point
    int16_t XCurrent, YCurrent;
    XCurrent = 0;
    YCurrent = Radius;

    // Cumulative error,judge the next point of the logo
    int16_t Esp = 3 - (Radius << 1);

    int16_t sCountY;
    if (Draw_Fill == DRAW_FILL_FULL) {
        while (XCurrent <= YCurrent) { // Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY++) {
                Paint_DrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color,
                                DOT_PIXEL_DFT,
                                DOT_STYLE_DFT); // 1
                Paint_DrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color,
                                DOT_PIXEL_DFT,
                                DOT_STYLE_DFT); // 2
                Paint_DrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color,
                                DOT_PIXEL_DFT,
                                DOT_STYLE_DFT); // 3
                Paint_DrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color,
                                DOT_PIXEL_DFT,
                                DOT_STYLE_DFT); // 4
                Paint_DrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color,
                                DOT_PIXEL_DFT,
                                DOT_STYLE_DFT); // 5
                Paint_DrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color,
                                DOT_PIXEL_DFT,
                                DOT_STYLE_DFT); // 6
                Paint_DrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color,
                                DOT_PIXEL_DFT,
                                DOT_STYLE_DFT); // 7
                Paint_DrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color,
                                DOT_PIXEL_DFT, DOT_STYLE_DFT);
            }
            if (Esp < 0)
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    } else { // Draw a hollow circle
        while (XCurrent <= YCurrent) {
            Paint_DrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color,
                            Line_width, DOT_STYLE_DFT); // 1
            Paint_DrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color,
                            Line_width, DOT_STYLE_DFT); // 2
            Paint_DrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color,
                            Line_width, DOT_STYLE_DFT); // 3
            Paint_DrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color,
                            Line_width, DOT_STYLE_DFT); // 4
            Paint_DrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color,
                            Line_width, DOT_STYLE_DFT); // 5
            Paint_DrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color,
                            Line_width, DOT_STYLE_DFT); // 6
            Paint_DrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color,
                            Line_width, DOT_STYLE_DFT); // 7
            Paint_DrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color,
                            Line_width, DOT_STYLE_DFT); // 0

            if (Esp < 0)
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent);
                YCurrent--;
            }
            XCurrent++;
        }
    }
}

/******************************************************************************
function: Show English characters
parameter:
    Xpoint           ：X coordinate
    Ypoint           ：Y coordinate
    Acsii_Char       ：To display the English characters
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawChar(uint16_t Xpoint, uint16_t Ypoint, const char Acsii_Char,
                    sFONT* Font, uint16_t Color_Foreground,
                    uint16_t Color_Background) {
    uint16_t Page, Column;
    char baseOffset;

    if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
        //  printf("Paint_DrawChar Input exceeds the normal display range\r\n");
        return;
    }

    if (Acsii_Char == ' ') {
        baseOffset = ' ';
    } else if (Acsii_Char == '.' || Acsii_Char == '-') {
        baseOffset = ',';
    } else if (Acsii_Char > 47 && Acsii_Char < 59) {
        baseOffset = '/' - 2;
    } else if (Acsii_Char > 64 && Acsii_Char < 91) {
        baseOffset = '@' - 13;
    } else if (Acsii_Char > 96 && Acsii_Char < 123) {
        baseOffset = '`' - 39;
    }
    uint32_t Char_Offset = (Acsii_Char - baseOffset) * Font->Height *
                           (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
    const unsigned char* ptr = &Font->table[Char_Offset];

    for (Page = 0; Page < Font->Height; Page++) {
        for (Column = 0; Column < Font->Width; Column++) {

            // To determine whether the font background color and screen
            // background color is consistent
            if (FONT_BACKGROUND ==
                Color_Background) { // this process is to speed up the scan
                if (*ptr & (0x80 >> (Column % 8)))
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page,
                                   Color_Foreground);
                // Paint_DrawPoint(Xpoint + Column, Ypoint + Page,
                // Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
            } else {
                if (*ptr & (0x80 >> (Column % 8))) {
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page,
                                   Color_Foreground);
                    // Paint_DrawPoint(Xpoint + Column, Ypoint + Page,
                    // Color_Foreground, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                } else {
                    Paint_SetPixel(Xpoint + Column, Ypoint + Page,
                                   Color_Background);
                    // Paint_DrawPoint(Xpoint + Column, Ypoint + Page,
                    // Color_Background, DOT_PIXEL_DFT, DOT_STYLE_DFT);
                }
            }
            // One pixel is 8 bits
            if (Column % 8 == 7)
                ptr++;
        } // Write a line
        if (Font->Width % 8 != 0)
            ptr++;
    } // Write all
}

/******************************************************************************
function:	Display the string
parameter:
    Xstart           ：X coordinate
    Ystart           ：Y coordinate
    pString          ：The first address of the English string to be displayed
    Font             ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawString_EN(uint16_t Xstart, uint16_t Ystart, const char* pString,
                         sFONT* Font, uint16_t Color_Foreground,
                         uint16_t Color_Background) {
    uint16_t Xpoint = Xstart;
    uint16_t Ypoint = Ystart;

    if (Xstart > Paint.Width || Ystart > Paint.Height) {
        //  printf("Paint_DrawString_EN Input exceeds the normal display
        //  range\r\n");
        return;
    }

    while (*pString != '\0') {
        // if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y
        // direction plus the Height of the character
        if ((Xpoint + Font->Width) > Paint.Width) {
            Xpoint = Xstart;
            Ypoint += Font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if ((Ypoint + Font->Height) > Paint.Height) {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }
        Paint_DrawChar(Xpoint, Ypoint, *pString, Font, Color_Background,
                       Color_Foreground);

        // The next character of the address
        pString++;

        // The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width;
    }
}

// /******************************************************************************
// function:	Display nummber
// parameter:
//     Xstart           ：X coordinate
//     Ystart           : Y coordinate
//     Nummber          : The number displayed
//     Font             ：A structure pointer that displays a character size
//     Color_Foreground : Select the foreground color
//     Color_Background : Select the background color
// ******************************************************************************/
uint8_t Paint_DrawNum(uint16_t Xpoint, uint16_t Ypoint, int32_t Number,
                      sFONT* Font, uint16_t Color_Foreground,
                      uint16_t Color_Background) {
    char Str[10];
    uint8_t size = inttostr(Number, Str);
    Paint_DrawString_EN(Xpoint, Ypoint, Str, Font, Color_Foreground,
                        Color_Background);
    return size;
}

// /******************************************************************************
// function:	Display time
// parameter:
//     Xstart           ：X coordinate
//     Ystart           : Y coordinate
//     pTime            : Time-related structures
//     Font             ：A structure pointer that displays a character size
//     Color_Foreground : Select the foreground color
//     Color_Background : Select the background color
// ******************************************************************************/

void Paint_DrawTime(uint16_t Xstart, uint16_t Ystart, uint8_t Hour, uint8_t Min,
                    sFONT* Font, uint16_t Color_Foreground,
                    uint16_t Color_Background) {
    // Clear buffer in specified range
    Paint_ClearWindows(5, 175, 5 + Font20.Width * 7, 175 + Font20.Height,
                       WHITE);

    // Write data into the cache
    Paint_DrawChar(Xstart, Ystart, num[Hour / 10], Font, Color_Background,
                   Color_Foreground);
    Paint_DrawChar(Xstart + Dx, Ystart, num[Hour % 10], Font, Color_Background,
                   Color_Foreground);
    Paint_DrawChar(Xstart + Dx + Dx_4 + Dx_2, Ystart, ':', Font,
                   Color_Background, Color_Foreground);
    Paint_DrawChar(Xstart + Dx * 2 + Dx_2, Ystart, num[Min / 10], Font,
                   Color_Background, Color_Foreground);
    Paint_DrawChar(Xstart + Dx * 3 + Dx_2, Ystart, num[Min % 10], Font,
                   Color_Background, Color_Foreground);
    Paint_DrawChar(Xstart + Dx * 4 + Dx_2, Ystart, 'P', Font, Color_Background,
                   Color_Foreground);
    Paint_DrawChar(Xstart + Dx * 5 + Dx_2, Ystart, 'M', Font, Color_Background,
                   Color_Foreground);
}

void Paint_DrawDate(uint16_t Xstart, uint16_t Ystart, uint8_t Month,
                    uint8_t Day, sFONT* Font, uint16_t Color_Foreground,
                    uint16_t Color_Background) {
    // Clear buffer in specified range
    Paint_ClearWindows(Xstart, Ystart, Xstart + Font20.Width * 6,
                       Ystart + Font20.Height, WHITE);

    char* months[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

    // Write data into the cache
    Paint_DrawChar(Xstart, Ystart, months[Month][0], Font, Color_Background,
                   Color_Foreground);
    Paint_DrawChar(Xstart + Dx, Ystart, months[Month][1], Font,
                   Color_Background, Color_Foreground);
    Paint_DrawChar(Xstart + Dx * 2, Ystart, months[Month][2], Font,
                   Color_Background, Color_Foreground);
    Paint_DrawChar(Xstart + Dx * 2 + Dx_4 + Dx_2, Ystart, ' ', Font,
                   Color_Background, Color_Foreground);
    Paint_DrawChar(Xstart + Dx * 3 + Dx_2, Ystart, num[Day / 10], Font,
                   Color_Background, Color_Foreground);
    Paint_DrawChar(Xstart + Dx * 4 + Dx_2, Ystart, num[Day % 10], Font,
                   Color_Background, Color_Foreground);
}

void Paint_DrawDistance(uint16_t Xstart, uint16_t Ystart, int distance) {
    Paint_ClearWindows(Xstart, Ystart, Xstart + Font20.Width * 5,
                       Ystart + Font20.Height, WHITE);

    int offset = Paint_DrawNum(Xstart, Ystart, distance, &Font20, WHITE, BLACK);
    Paint_DrawChar(Xstart + Font20.Width * offset, Ystart, 'm', &Font20, BLACK,
                   WHITE);
}

void Paint_DrawTemp(uint16_t Xstart, uint16_t Ystart, int temp) {
    Paint_ClearWindows(Xstart, Ystart - Dx_2, Xstart + Font20.Width * 4,
                       Ystart + Font20.Height, WHITE);

    uint8_t offset = Paint_DrawNum(Xstart, Ystart, temp, &Font20, WHITE, BLACK);
    Paint_DrawCircle(Xstart + Font20.Width * offset + 6,
                     Ystart + Font20.Width / 4, 2, BLACK, DOT_PIXEL_1X1,
                     DRAW_FILL_EMPTY);
    Paint_DrawChar(Xstart + Font20.Width * offset + Dx_2, Ystart, 'C', &Font20,
                   BLACK, WHITE);
}

void Paint_DrawLatLon(uint16_t Xstart, uint16_t Ystart, int32_t lat,
                      int32_t lon) {
    Paint_ClearWindows(10, Ystart, 195, Ystart + 18 + Font20.Height, WHITE);

    Paint_DrawString_EN(Xstart, Ystart, "Lat:", &Font20, WHITE, BLACK);
    Paint_DrawString_EN(Xstart, Ystart + 20, "Lon:", &Font20, WHITE, BLACK);

    uint8_t latOffset = 0, lonOffset = 0;
    if (lat < 0) {
        latOffset = Paint_DrawNum(Xstart + Font20.Width * 4, Ystart,
                                  lat / 10000000, &Font20, WHITE, BLACK);
        lat = -lat;
    } else {
        latOffset = Paint_DrawNum(Xstart + Font20.Width * 5, Ystart,
                                  lat / 10000000, &Font20, WHITE, BLACK);
        ++latOffset;
    }

    if (lon < 0) {
        lonOffset = Paint_DrawNum(Xstart + Font20.Width * 4, Ystart + 20,
                                  lon / 10000000, &Font20, WHITE, BLACK);
        lon = -lon;
    } else {
        lonOffset = Paint_DrawNum(Xstart + Font20.Width * 5, Ystart + 20,
                                  lon / 10000000, &Font20, WHITE, BLACK);
        ++lonOffset;
    }

    Paint_DrawChar(Xstart + Font20.Width * (4 + latOffset), Ystart, '.',
                   &Font20, BLACK, WHITE);
    Paint_DrawChar(Xstart + Font20.Width * (4 + lonOffset), Ystart + 20, '.',
                   &Font20, BLACK, WHITE);

    Paint_DrawChar(Xstart + Font20.Width * (5 + latOffset), Ystart,
                   num[(lat / 1000000) % 10], &Font20, BLACK, WHITE);
    Paint_DrawChar(Xstart + Font20.Width * (6 + latOffset), Ystart,
                   num[(lat / 100000) % 10], &Font20, BLACK, WHITE);
    Paint_DrawChar(Xstart + Font20.Width * (7 + latOffset), Ystart,
                   num[(lat / 10000) % 10], &Font20, BLACK, WHITE);
    Paint_DrawChar(Xstart + Font20.Width * (8 + latOffset), Ystart,
                   num[(lat / 1000) % 10], &Font20, BLACK, WHITE);

    Paint_DrawChar(Xstart + Font20.Width * (5 + lonOffset), Ystart + 20,
                   num[(lon / 1000000) % 10], &Font20, BLACK, WHITE);
    Paint_DrawChar(Xstart + Font20.Width * (6 + lonOffset), Ystart + 20,
                   num[(lon / 100000) % 10], &Font20, BLACK, WHITE);
    Paint_DrawChar(Xstart + Font20.Width * (7 + lonOffset), Ystart + 20,
                   num[(lon / 10000) % 10], &Font20, BLACK, WHITE);
    Paint_DrawChar(Xstart + Font20.Width * (8 + lonOffset), Ystart + 20,
                   num[(lon / 1000) % 10], &Font20, BLACK, WHITE);
}

void Paint_DrawOutline() {
    Paint_DrawLine(2, 165, 198, 165, BLACK, DOT_PIXEL_2X2, LINE_STYLE_SOLID);
}

inline point_t rot(point_t p, point_t c, float cos_theta, float sin_theta) {
    float x = p.x;
    float y = p.y;
    float cx = c.x;
    float cy = c.y;

    p.x = x * cos_theta - cx * cos_theta - y * sin_theta + cy * sin_theta + cx;
    p.y = y * cos_theta - cy * cos_theta + x * sin_theta - cx * sin_theta + cy;
    return p;
}

void Paint_DrawArrowd(const uint16_t angle_deg) {
    const float cx = 65.f; // center x
    const float cy = 65.f; // center y
    const uint16_t padding =
        30; // padding between arrow and quarter-screen edges
    const uint16_t arrow_len = 70; // Arrow length
    const uint16_t arrow_tip_len = 15;
    float theta = (float)angle_deg / 180.f * M_PI;
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);

    // counters
    uint16_t i;

    // point coordinates
    point_t p1, p2;
    point_t center = {cx, cy};

    // Draw arrow shaft
    for (i = 0; i < arrow_len; ++i) {
        // Generate a point (x, y) for a pixel on the arrow
        p1.x = padding + i;
        p1.y = cx;

        // Rotate pixel
        p1 = rot(p1, center, cos_theta, sin_theta);

        // Draw pixel
        Paint_DrawPoint(p1.y, p1.x, BLACK, DOT_PIXEL_2X2, DOT_FILL_AROUND);
    }

    // Draw arrow point
    for (i = 0; i < arrow_tip_len; ++i) {
        // Generate a point (x, y) for a pixel on the arrow
        p1.x = padding + i;
        p2.x = padding + i;
        p1.y = cx + i;
        p2.y = cx - i;

        // Rotate pixel
        p1 = rot(p1, center, cos_theta, sin_theta);
        p2 = rot(p2, center, cos_theta, sin_theta);

        // Draw pixel
        Paint_DrawPoint(p1.y, p1.x, BLACK, DOT_PIXEL_2X2, DOT_FILL_AROUND);
        Paint_DrawPoint(p2.y, p2.x, BLACK, DOT_PIXEL_2X2, DOT_FILL_AROUND);
    }
}

void Paint_DrawNorth(const uint16_t angle_deg) {
    const float cx = 65.f; // center x
    const float cy = 65.f; // center y
    const uint16_t padding =
        14; // padding between arrow and quarter-screen edges
    const uint16_t arrow_len = 80; // Arrow length
    const uint16_t arrow_tip_len = 10;
    float theta = (float)angle_deg / 180.f * M_PI;
    float cos_theta = cosf(theta);
    float sin_theta = sinf(theta);

    // counters
    uint16_t i;

    // point coordinates
    point_t p1, p2;
    point_t center = {cx, cy};

    // Draw arrow point
    for (i = 0; i < arrow_tip_len; ++i) {
        // Generate a point (x, y) for a pixel on the arrow
        p1.x = padding + i;
        p2.x = padding + i;
        p1.y = cx + i;
        p2.y = cx - i;

        // Rotate pixel
        p1 = rot(p1, center, cos_theta, sin_theta);
        p2 = rot(p2, center, cos_theta, sin_theta);

        // Draw pixel
        Paint_DrawPoint(p1.y, p1.x, BLACK, DOT_PIXEL_2X2, DOT_FILL_AROUND);
        Paint_DrawPoint(p2.y, p2.x, BLACK, DOT_PIXEL_2X2, DOT_FILL_AROUND);
        Paint_DrawLine(p1.y, p1.x, p2.y, p2.x, BLACK, DOT_PIXEL_2X2,
                       LINE_STYLE_SOLID);
    }
}
