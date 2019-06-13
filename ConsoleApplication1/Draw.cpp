#include "VectorXD.h"
#include<windows.h>
#include "Draw.h"
namespace maki {
	int screenWidth = 20;
	int screenHeight = 20;
	int maxClip_x = screenWidth - 1;
	int maxClip_y = screenHeight - 1;
	int minClip_x = 0;
	int minClip_y = 0;
	int byteStep = 3;

	void PrintTest(int *begin, int total,int color) {
		std::cout << std::endl;
		for (int i = 0; i < total; ++i) {
			auto c = *begin;
			if (c == color) {
				std::cout << "X";
			}
			else {
				std::cout << "o";
			}
			if ((i + 1) % screenWidth == 0 and i != 0) {
				std::cout << std::endl;
			}
			begin += 1;
		}
		std::cout << std::endl;
	}
	// 线段2D裁剪
	bool ClipLine(Vector4D &pt1,Vector4D &pt2) {
		// 参考https://blog.csdn.net/cppyin/article/details/6172457
#define CLIP_CODE_C  0x0000
#define CLIP_CODE_N  0x0008
#define CLIP_CODE_S  0x0004
#define CLIP_CODE_E  0x0002
#define CLIP_CODE_W  0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009 
#define CLIP_CODE_SW 0x0005

		int xc1 = pt1.x,
			yc1 = pt1.y,
			xc2 = pt2.x,
			yc2 = pt2.y;

		int p1_code = 0,
			p2_code = 0;

		// determine codes for p1 and p2
		if (pt1.y < minClip_y)
			p1_code |= CLIP_CODE_N;
		else
			if (pt1.y > maxClip_y)
				p1_code |= CLIP_CODE_S;

		if (pt1.x < minClip_x)
			p1_code |= CLIP_CODE_W;
		else
			if (pt1.x > maxClip_x)
				p1_code |= CLIP_CODE_E;

		if (pt2.y < minClip_y)
			p2_code |= CLIP_CODE_N;
		else
			if (pt2.y > maxClip_y)
				p2_code |= CLIP_CODE_S;

		if (pt2.x < minClip_x)
			p2_code |= CLIP_CODE_W;
		else
			if (pt2.x > maxClip_x)
				p2_code |= CLIP_CODE_E;

		// 全部在同一侧，完全裁剪
		if ((p1_code & p2_code) == 1)
			return false;

		// 不需要裁剪，完全保留
		if (p1_code == 0 && p2_code == 0)
			return true;

		// determine end clip point for p1点斜式： y - y1 = m * (x - x1)
		switch (p1_code) {
		case CLIP_CODE_C: break;

		case CLIP_CODE_N:
		{
			yc1 = minClip_y;
			xc1 = pt1.x + 0.5 + (minClip_y - pt1.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);//+0.5是为了转成整数时四舍五入
		} break;
		case CLIP_CODE_S:
		{
			yc1 = maxClip_y;
			xc1 = pt1.x + 0.5 + (maxClip_y - pt1.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);
		} break;

		case CLIP_CODE_W:
		{

		} break;

		case CLIP_CODE_E:
		{
			xc1 = maxClip_x;
			yc1 = pt1.y + 0.5 + (maxClip_x - pt1.x)*(pt2.y - pt1.y) / (pt2.x - pt1.x);
		} break;

		// these cases are more complex, must compute 2 intersections
		case CLIP_CODE_NE:
		{
			// north hline intersection
			yc1 = minClip_y;
			xc1 = pt1.x + 0.5 + (minClip_y - pt1.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);

			// test if intersection is valid, of so then done, else compute next
			if (xc1 < minClip_x || xc1 > maxClip_x)
			{
				// east vline intersection
				xc1 = maxClip_x;
				yc1 = pt1.y + 0.5 + (maxClip_x - pt1.x)*(pt2.y - pt1.y) / (pt2.x - pt1.x);
			} // end if

		} break;

		case CLIP_CODE_SE:
		{
			// south hline intersection
			yc1 = maxClip_y;
			xc1 = pt1.x + 0.5 + (maxClip_y - pt1.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);

			// test if intersection is valid, of so then done, else compute next
			if (xc1 < minClip_x || xc1 > maxClip_x)
			{
				// east vline intersection
				xc1 = maxClip_x;
				yc1 = pt1.y + 0.5 + (maxClip_x - pt1.x)*(pt2.y - pt1.y) / (pt2.x - pt1.x);
			} // end if

		} break;

		case CLIP_CODE_NW:
		{
			// north hline intersection
			yc1 = minClip_y;
			xc1 = pt1.x + 0.5 + (minClip_y - pt1.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);

			// test if intersection is valid, of so then done, else compute next
			if (xc1 < minClip_x || xc1 > maxClip_x)
			{
				xc1 = minClip_x;
				yc1 = pt1.y + 0.5 + (minClip_x - pt1.x)*(pt2.y - pt1.y) / (pt2.x - pt1.x);
			} // end if

		} break;

		case CLIP_CODE_SW:
		{
			// south hline intersection
			yc1 = maxClip_y;
			xc1 = pt1.x + 0.5 + (maxClip_y - pt1.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);

			// test if intersection is valid, of so then done, else compute next
			if (xc1 < minClip_x || xc1 > maxClip_x)
			{
				xc1 = minClip_x;
				yc1 = pt1.y + 0.5 + (minClip_x - pt1.x)*(pt2.y - pt1.y) / (pt2.x - pt1.x);
			} // end if

		} break;

		default:break;

		} // end switch

		// determine clip point for p2
		switch (p2_code) {
		case CLIP_CODE_C: break;

		case CLIP_CODE_N:
		{
			yc2 = minClip_y;
			xc2 = pt2.x + 0.5 + (minClip_y - pt2.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);//+0.5是为了转成整数时四舍五入
		} break;
		case CLIP_CODE_S:
		{
			yc2 = maxClip_y;
			xc2 = pt2.x + 0.5 + (maxClip_y - pt2.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);
		} break;

		case CLIP_CODE_W:
		{

		} break;

		case CLIP_CODE_E:
		{
			xc2 = maxClip_x;
			yc2 = pt2.y + 0.5 + (maxClip_x - pt2.x)*(pt2.y - pt1.y) / (pt2.x - pt1.x);
		} break;

		// these cases are more complex, must compute 2 intersections
		case CLIP_CODE_NE:
		{
			// north hline intersection
			yc2 = minClip_y;
			xc2 = pt2.x + 0.5 + (minClip_y - pt2.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);

			// test if intersection is valid, of so then done, else compute next
			if (xc2 < minClip_x || xc2 > maxClip_x)
			{
				// east vline intersection
				xc2 = maxClip_x;
				yc2 = pt2.y + 0.5 + (maxClip_x - pt2.x)*(pt2.y - pt1.y) / (pt2.x - pt1.x);
			} // end if

		} break;

		case CLIP_CODE_SE:
		{
			// south hline intersection
			yc2 = maxClip_y;
			xc2 = pt2.x + 0.5 + (maxClip_y - pt2.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);

			// test if intersection is valid, of so then done, else compute next
			if (xc2 < minClip_x || xc2 > maxClip_x)
			{
				// east vline intersection
				xc2 = maxClip_x;
				yc2 = pt2.y + 0.5 + (maxClip_x - pt2.x)*(pt2.y - pt1.y) / (pt2.x - pt1.x);
			} // end if

		} break;

		case CLIP_CODE_NW:
		{
			// north hline intersection
			yc2 = minClip_y;
			xc2 = pt1.x + 0.5 + (minClip_y - pt1.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);

			// test if intersection is valid, of so then done, else compute next
			if (xc2 < minClip_x || xc2 > maxClip_x)
			{
				xc2 = minClip_x;
				yc2 = pt2.y + 0.5 + (minClip_x - pt2.x)*(pt2.y - pt1.y) / (pt2.x - pt1.x);
			} // end if

		} break;

		case CLIP_CODE_SW:
		{
			// south hline intersection
			yc2 = maxClip_y;
			xc2 = pt2.x + 0.5 + (maxClip_y - pt2.y)*(pt2.x - pt1.x) / (pt2.y - pt1.y);

			// test if intersection is valid, of so then done, else compute next
			if (xc2 < minClip_x || xc2 > maxClip_x)
			{
				xc2 = minClip_x;
				yc2 = pt2.y + 0.5 + (minClip_x - pt2.x)*(pt2.y - pt1.y) / (pt2.x - pt1.x);
			} // end if

		} break;

		default:break;

		} // end switch

		// do bounds check
		if ((xc1 < minClip_x) || (xc1 > maxClip_x) ||
			(yc1 < minClip_y) || (yc1 > maxClip_y) ||
			(xc2 < minClip_x) || (xc2 > maxClip_x) ||
			(yc2 < minClip_y) || (yc2 > maxClip_y))
		{
			return false;
		} // end if

	// store vars back
		pt1.x = xc1;
		pt1.y = yc1;
		pt2.x = xc2;
		pt2.y = yc2;

		return true;

	} // end Clip_Line

	//画线 Bresenham算法参考 ：https://blog.csdn.net/Jurbo/article/details/52653276 https://zhuanlan.zhihu.com/p/20213658 
	int DrawLine(Vector4D &start, Vector4D &end,int color, uchar *framePtr, int lpitch) {//framePtr帧缓存起始地址，lpitch每行像素
		
		int dx,             // difference in x's
			dy,             // difference in y's
			dx2,            // dx,dy * 2
			dy2,
			x_inc,          // amount in pixel space to move during drawing
			y_inc,          // amount in pixel space to move during drawing
			error,          // the discriminant i.e. error i.e. decision variable
			index;          // used for looping

		// compute horizontal and vertical deltas
		dx = end.x - start.x;
		dy = end.y - start.y;

		// test which direction the line is going in i.e. slope angle
		if (dx >= 0)
		{
			x_inc = byteStep;

		} // end if line is moving right
		else
		{
			x_inc = -byteStep;
			dx = -dx;  // need absolute value

		} // end else moving left

	 // test y component of slope

		if (dy >= 0)
		{
			y_inc = lpitch;
		} // end if line is moving down
		else
		{
			y_inc = -lpitch;
			dy = -dy;  // need absolute value

		} // end else moving up

	 // compute (dx,dy) * 2
		dx2 = dx << 1;
		dy2 = dy << 1;

		//斜率 <1
		if (dx > dy){
			// 将点代入Ax+By+C，等于0时点在直线上，大于0时在直线的一侧，小于0在另一侧
			error = dy2 - dx;

			// draw the line
			for (index = 0; index <= dx; index++)
			{
				// set the pixel
				*(int *)framePtr = color;
				// test if error has overflowed
				if (error >= 0)
				{
					error -= dx2;

					// move to next line
					framePtr += y_inc;

				} // end if error overflowed

				// adjust the error term
				error += dy2;

				// move to the next pixel
				framePtr += x_inc;

			} // end for

		} // end if |slope| <= 1
		else
		{
			// initialize error term
			error = dx2 - dy;

			// draw the line
			for (index = 0; index <= dy; index++)
			{
				// set the pixel
				*(int *)framePtr = color;
				// test if error overflowed
				if (error >= 0)
				{
					error -= dy2;

					// move to next line
					framePtr += x_inc;

				} // end if error overflowed

			 // adjust the error term
				error += dx2;

				// move to the next pixel
				framePtr += y_inc;

			} // end for

		} // end else |slope| > 1

	 // return success
		return true;

	} // end Draw_Line

	// 结合2d裁剪，画线(颜色存储int32)
	void DrawClipLine(const Vector4D &pt1, const Vector4D &pt2, int color,uchar *dest_buffer, int lpitch){
		Vector4D tPt1 = pt1;
		tPt1.x = tPt1.x + 0.5;
		tPt1.y = tPt1.y + 0.5;
		Vector4D tPt2 = pt2;
		tPt2.x = tPt2.x + 0.5;
		tPt2.y = tPt2.y + 0.5;
		// clip the line
		if (ClipLine(tPt1, tPt2)) {
			auto step = (int)(tPt1.y) * lpitch + (int)(tPt1.x) * 4;
			dest_buffer += step;
			DrawLine(tPt1, tPt2, color, dest_buffer, lpitch);
		}

	} // end DrawClipLine

	void DrawLineHorizontal(float xs,float xe,uchar *destAddr,int color) {
		int start = (int)(xs + 0.5);
		int end = (int)(xe + 0.5);
		for (; start < end; ++start, destAddr += byteStep) {
			*(int *)destAddr = color;
		}
	}

	// 平顶三角形
	bool DrawTopTri(float x1, float y1,//顶点1,2,3需要遵从一定是顺序
		float x2, float y2,
		float x3, float y3,
		int color,
		uchar *framePtr, int lpitch)
	{
		// destination address of next scanline
		uchar  *destAddr = framePtr;

		// test order of x1 and x2
		if (x2 < x1)
		{
			float temp_x = x2;
			x2 = x1;
			x1 = temp_x;
		} // end if swap

	 // compute delta's
		float height = y3 - y1;
		if (fabs(height) < 0.00001) {
			return false;
		}
		float dx_left = (x3 - x1) / height;
		float dx_right = (x3 - x2) / height;

		// set starting points
		float xs = x1;
		float xe = x2;

		// perform y clipping
		if (y1 < minClip_x)
		{
			// compute new xs and ys
			xs = xs + dx_left * (-y1 + minClip_y);
			xe = xe + dx_right * (-y1 + minClip_y);
			// reset y1
			y1 = minClip_y;

		} // end if top is off screen

		if (y3 > maxClip_y)
			y3 = maxClip_y;

		// compute starting address in video memory
		destAddr = destAddr + (int)(y1+0.5) * lpitch;

		int max_y = (int)(y3 + 0.5);
		// test if x clipping is needed
		if (x1 >= minClip_x && x1 <= maxClip_x &&
			x2 >= minClip_x && x2 <= maxClip_x &&
			x3 >= minClip_x && x3 <= maxClip_x)
		{
			// draw the triangle
			for (int temp_y = y1; temp_y <= max_y; ++temp_y, destAddr += lpitch)
			{
				memset(destAddr + (unsigned int)xs, color, (unsigned int)((int)xe - (int)xs + 1));
				
				// adjust starting point and ending point
				xs += dx_left;
				xe += dx_right;

			} // end for

		} // end if no x clipping needed
		else
		{
			// clip x axis with slower version
			for (int temp_y = y1; temp_y <= max_y; ++temp_y, destAddr += lpitch)
			{
				// do x clip
				int left = (int)xs;
				int right = (int)xe;

				// adjust starting point and ending point
				xs += dx_left;
				xe += dx_right;

				// clip line
				if (left < minClip_x)
				{
					left = minClip_x;

					if (right < minClip_x)
						continue;
				}

				if (right > maxClip_x)
				{
					right = maxClip_x;

					if (left > maxClip_x)
						continue;
				}

				memset(destAddr + (unsigned int)left, color, (unsigned int)(right - left + 1));

			} // end for

		} // end else x clipping needed
		return true;
	} // end Draw_Top_Tri

}