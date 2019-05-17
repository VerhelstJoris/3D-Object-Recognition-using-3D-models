

namespace ImageOperations //optional, just for clarity
{
	static cv::Mat& ScanImageAndReduceC(cv::Mat& I, const uchar* const table)
	{
		// accept only char type matrices
		CV_Assert(I.depth() == CV_8U);
		int channels = I.channels();
		int nRows = I.rows;
		int nCols = I.cols * channels;
		if (I.isContinuous())
		{
			nCols *= nRows;
			nRows = 1;
		}
		int i, j;
		uchar* p;
		for (i = 0; i < nRows; ++i)
		{
			p = I.ptr<uchar>(i);
			for (j = 0; j < nCols; ++j)
			{
				p[j] = table[p[j]];
			}
		}
		return I;
	}

}