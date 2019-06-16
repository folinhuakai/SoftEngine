#include "VectorXD.h"
#include<windows.h>
#include "Draw.h"
#include "Math.h"
namespace maki {
	int screenWidth = 20;
	int screenHeight = 20;
	int maxClip_x = screenWidth - 1;
	int maxClip_y = screenHeight - 1;
	int minClip_x = 0;
	int minClip_y = 0;
	int byteStep = 4;

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


	// 平顶三角形
	bool DrawTopTri(int x1, int y1,//顶点1,2,3需要遵从一定是顺序
		int x2, int y2,
		int x3, int y3,
		int color,
		uchar *framePtr, int lpitch)
	{
		// test for degenerate
		if (y1==y3 || y2==y3)
			return false;
		// destination address of next scanline
		uchar  *destAddr = framePtr;

		// test order of x1 and x2
		if (x2 < x1)
		{
			int temp_x = x2;
			x2 = x1;
			x1 = temp_x;
		} // end if swap

	 // compute delta's
		float height = y3 - y1;
		float dx_left = (x3 - x1) / height;
		float dx_right = (x3 - x2) / height;

		// set starting points
		float xs = (float) x1;
		float xe = (float) x2;

		// perform y clipping
		if (y1 < minClip_y)
		{
			// compute new xs and ys
			xs = xs + dx_left * (float)(-y1 + minClip_y);
			xe = xe + dx_right * (float)(-y1 + minClip_y);
			// reset y1
			y1 = minClip_y;

		} // end if top is off screen

		if (y3 > maxClip_y)
			y3 = maxClip_y;

		// compute starting address in video memory
		destAddr = destAddr + y1 * lpitch;

		// test if x clipping is needed
		if (x1 + 1 >= minClip_x && x1 + 1 <= maxClip_x &&
			x2 + 1 >= minClip_x && x2 + 1 <= maxClip_x &&
			x3 + 1 >= minClip_x && x3 + 1 <= maxClip_x)
		{
			// draw the triangle
			for (int temp_y = y1; temp_y <= y3; ++temp_y, destAddr += lpitch)
			{

				DrawLineHorizontal(xs + 0.5, xe + 0.5, destAddr, color);

				// adjust starting point and ending point
				xs += dx_left;
				xe += dx_right;

			} // end for

		} // end if no x clipping needed
		else
		{
			// clip x axis with slower version
			for (int temp_y = y1; temp_y <= y3; ++temp_y, destAddr += lpitch)
			{
				// do x clip
				int left = (int)(xs + 0.5);
				int right = (int)(xe + 0.5);

				// adjust starting point and ending point
				xs += (dx_left);
				xe += (dx_right);

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

				DrawLineHorizontal(left, right, destAddr, color);

			} // end for

		} // end else x clipping needed
		return true;
	} // end Draw_Top_Tri

	// 平底三角形
	bool DrawBottomTri(int x1, int y1, int x2, int y2, int x3, int y3, int color, uchar *framePtr, int lpitch) {
			if (y1 == y2 || y1 == y3)
				return false;

			uchar  *destAddr = framePtr;

			// test order of x1 and x2
			if (x3 < x2)
			{
				int temp_x = x2;
				x2 = x3;
				x3 = temp_x;

			} // end if swap

		 // compute delta's
			float height = y3 - y1;

			float dx_left = (x2 - x1) / height;
			float dx_right = (x3 - x1) / height;

			// set starting points
			float xs = x1;
			float xe = x1;

			// perform y clipping
			if (y1 < minClip_y)
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
			destAddr = destAddr + y1 * lpitch;
			// test if x clipping is needed
			if (x1 + 1 >= minClip_x && x1 + 1 <= maxClip_x &&
				x2 + 1 >= minClip_x && x2 + 1 <= maxClip_x &&
				x3 + 1 >= minClip_x && x3 + 1 <= maxClip_x)
			{
				// draw the triangle
				for (int temp_y = y1; temp_y <= y3; ++temp_y, destAddr += lpitch)
				{

					DrawLineHorizontal(xs + 0.5, xe + 0.5, destAddr, color);

					// adjust starting point and ending point
					xs += dx_left;
					xe += dx_right;

				} // end for

			} // end if no x clipping needed
			else
			{
				// clip x axis with slower version
				for (int temp_y = y1; temp_y <= y3; ++temp_y, destAddr += lpitch)
				{
					// do x clip
					int left = (int)(xs + 0.5);
					int right = (int)(xe + 0.5);

					// adjust starting point and ending point
					xs += (dx_left);
					xe += (dx_right);

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

					DrawLineHorizontal(left, right, destAddr, color);

				} // end for

			} // end else x clipping needed
			return true;

		} // end Draw_Bottom_TriFP

	// 任意形状三角形
	bool DrawTriangle(const Vector4D &pt1, const Vector4D &pt2, const Vector4D &pt3,int color,uchar *dest_buffer, int lpitch){
		int x1 = pt1.x +0.5;
		int y1 = pt1.y + 0.5;
		int x2 = pt2.x + 0.5;
		int y2 = pt2.y + 0.5;
		int x3 = pt3.x + 0.5;
		int y3 = pt3.y + 0.5;

		// test for h lines and v lines
		if ((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3))
			return false;

		int temp_x, temp_y;
		// sort p1,p2,p3 in ascending y order
		if (y2 < y1)
		{
			temp_x = x2;
			temp_y = y2;
			x2 = x1;
			y2 = y1;
			x1 = temp_x;
			y1 = temp_y;
		} 

		if (y3 < y1)
		{
			temp_x = x3;
			temp_y = y3;
			x3 = x1;
			y3 = y1;
			x1 = temp_x;
			y1 = temp_y;
		} 

		if (y3 < y2)
		{
			temp_x = x3;
			temp_y = y3;
			x3 = x2;
			y3 = y2;
			x2 = temp_x;
			y2 = temp_y;

		} // end if

		 // do trivial rejection tests for clipping
		if (y3<minClip_y || y1>maxClip_y ||
			(x1 < minClip_x && x2 < minClip_x && x3 < minClip_x) ||
			(x1 > maxClip_x && x2 > maxClip_x && x3 > maxClip_x))
			return false;

		// test if top of triangle is flat
		if (y1 == y2)
		{
			return DrawTopTri(x1, y1, x2, y2, x3, y3, color, dest_buffer, lpitch);
		} // end if
		else
			if (y2 == y3)
			{
				return DrawBottomTri(x1, y1, x2, y2, x3, y3, color, dest_buffer, lpitch);
			} // end if bottom is flat
			else
			{
				// general triangle that's needs to be broken up along long edge
				int new_x = x1 + (int)(0.5 + (float)(y2 - y1)*(float)(x3 - x1) / (float)(y3 - y1));

				// draw each sub-triangle
				bool isTrue = DrawBottomTri(x1, y1, new_x, y2, x2, y2, color, dest_buffer, lpitch);
				if (isTrue) {
					return DrawTopTri(x2, y2, new_x, y2, x3, y3, color, dest_buffer, lpitch);
				}
				else {
					return false;
				}
				
			} 

	} // end
}