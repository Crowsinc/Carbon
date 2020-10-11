#include "BinPacking.hpp"

#include <algorithm>
#include <functional>

#include "../Utility/VectorExtensions.hpp"
#include "../Diagnostics/Assert.hpp"

namespace cbn
{
	//-------------------------------------------------------------------------------------

	std::tuple<int, int> best_short_side_fit(const Rect<int> bin, const Rect<int> rect)
	{
		const int horizontal_space = std::abs(bin.width - rect.width);
		const int vertical_space = std::abs(bin.height - rect.height);
		return {std::min(vertical_space, horizontal_space), 0};
	}

	//-------------------------------------------------------------------------------------

	std::tuple<int, int> best_long_side_fit(const Rect<int> bin, const Rect<int> rect)
	{
		const int horizontal_space = std::abs(bin.width - rect.width);
		const int vertical_space = std::abs(bin.height - rect.height);
		return {std::max(vertical_space, horizontal_space), 0};
	}

	//-------------------------------------------------------------------------------------

	std::tuple<int, int> best_area_fit(const Rect<int> bin, const Rect<int> rect)
	{
		const int empty_area = (bin.width * bin.height) - (rect.width * rect.height);
		return {empty_area, 0};
	}

	//-------------------------------------------------------------------------------------

	std::tuple<int, int> best_short_side_fit_blsf(const Rect<int> bin, const Rect<int> rect)
	{
		return {std::get<0>(best_short_side_fit(bin, rect)), std::get<0>(best_long_side_fit(bin, rect))};
	}

	//-------------------------------------------------------------------------------------

	std::tuple<int, int> best_long_side_fit_bssf(const Rect<int> bin, const Rect<int> rect)
	{
		return {std::get<0>(best_long_side_fit(bin, rect)), std::get<0>(best_short_side_fit(bin, rect))};
	}

	//-------------------------------------------------------------------------------------

	std::tuple<int, int> best_area_fit_bssf(const Rect<int> bin, const Rect<int> rect)
	{
		return {std::get<0>(best_area_fit(bin, rect)), std::get<0>(best_short_side_fit_blsf(bin, rect))};
	}
	
	//-------------------------------------------------------------------------------------

	std::tuple<int, int> best_area_fit_blsf(const Rect<int> bin, const Rect<int> rect)
	{
		return {std::get<0>(best_area_fit(bin, rect)), std::get<0>(best_long_side_fit_bssf(bin, rect))};
	}

	//-------------------------------------------------------------------------------------

	std::tuple<int, int> best_short_side_fit_baf(const Rect<int> bin, const Rect<int> rect)
	{
		return {std::get<0>(best_short_side_fit(bin, rect)), std::get<0>(best_area_fit(bin, rect))};
	}

	//-------------------------------------------------------------------------------------

	std::tuple<int, int> best_long_side_fit_baf(const Rect<int> bin, const Rect<int> rect)
	{
		return {std::get<0>(best_long_side_fit_bssf(bin, rect)), std::get<0>(best_area_fit(bin, rect))};
	}
	
	//-------------------------------------------------------------------------------------

	int calculate_sqr_score_penalty(const Rect<int> bin, const Rect<int> rect)
	{
		constexpr const float weight = 5;
		
		// To make sure that we get a square footprint. We want to penalise
		// the algorithm for using bins which are too far in one direction.
		// Hence we will take the maximum coord and combine it with a weight,
		// to get the penalty. The weight will affect whether we care more about
		// the square footprint, or how well the rect fits in the bin with the
		// heuristic bein used. Note that it may be more intelligent to not
		// just take the bin x and y, but also the rectangles width and height
		// into account, so that we can penalise rects which are rotated in a 
		// way which increases the square size more. Attempts of this so far 
		// have not worked out. It may be because a bad rotation is penalised
		// by this and the heuristic at the same time, so rotations are favoured
		// which means too much stuff is rotated, so the packing quality suffers. 

		return std::round(std::max(bin.x, bin.y) * weight);
	}
	
	//-------------------------------------------------------------------------------------

	bool has_sqr_heuristic(const RectanglePackingHeuristic heuristic)
	{
		switch(heuristic)
		{
			case RectanglePackingHeuristic::BLSF_BSSF_SQR:
			case RectanglePackingHeuristic::BSSF_BLSF_SQR:
			case RectanglePackingHeuristic::BLSF_BAF_SQR:
			case RectanglePackingHeuristic::BSSF_BAF_SQR:
			case RectanglePackingHeuristic::BSSF_SQR:
			case RectanglePackingHeuristic::BLSF_SQR:
				return true;
			default:
				return false;
		}
	}

	//-------------------------------------------------------------------------------------


	std::tuple<int, int> calculate_score(const Rect<int> bin, const Rect<int> rect, const RectanglePackingHeuristic heuristic)
	{
		std::tuple<int, int> scores;

		switch(heuristic)
		{
			case RectanglePackingHeuristic::BAF:
				scores = best_area_fit(bin, rect);
				break;
			case RectanglePackingHeuristic::BSSF:
			case RectanglePackingHeuristic::BSSF_SQR:
				scores = best_short_side_fit(bin, rect);
				break;
			case RectanglePackingHeuristic::BLSF:
			case RectanglePackingHeuristic::BLSF_SQR:
				scores = best_short_side_fit(bin, rect);
				break;
			case RectanglePackingHeuristic::BSSF_BLSF:
			case RectanglePackingHeuristic::BSSF_BLSF_SQR:
				scores = best_short_side_fit_blsf(bin, rect);
				break;
			case RectanglePackingHeuristic::BLSF_BSSF:
			case RectanglePackingHeuristic::BLSF_BSSF_SQR:
				scores = best_long_side_fit_bssf(bin, rect);
				break;
			case RectanglePackingHeuristic::BAF_BSSF:
				scores = best_area_fit_bssf(bin, rect);
				break;
			case RectanglePackingHeuristic::BAF_BLSF:
				scores = best_area_fit_blsf(bin, rect);
				break;
			case RectanglePackingHeuristic::BSSF_BAF:
			case RectanglePackingHeuristic::BSSF_BAF_SQR:
				scores = best_short_side_fit_baf(bin, rect);
				break;
			case RectanglePackingHeuristic::BLSF_BAF:
			case RectanglePackingHeuristic::BLSF_BAF_SQR:
				scores = best_long_side_fit_baf(bin, rect);
				break;
			default:
				return {0,0};
		}

		// If we are also using the SQR heuristic, we 
		// need to calculate a score penalty for it
		if(has_sqr_heuristic(heuristic))
			std::get<0>(scores) += calculate_sqr_score_penalty(bin, rect);

		return scores;
	}
	
	//-------------------------------------------------------------------------------------

	void split_bin(const Rect<int> bin, const Rect<int> rect, std::vector<Rect<int>>& bins, const int minimum_dimension)
	{
		CBN_Assert(intersects(bin, rect), "Cannot split bin if it does not intersect with rect");
		
		// If the top of the rectangle is under the top of the bin
		// then we can make a maximal bin on top of the bin expanding horizontally
		if(rect.y > bin.y)
		{
			const Rect<int> new_bin = {bin.x, bin.y, bin.width, rect.y - bin.y};

			if(new_bin.width >= minimum_dimension && new_bin.height >= minimum_dimension)
				bins.push_back(new_bin);
		}

		// If the bottom of the rectangle is over the bottom of the bin
		// then we can make a maximal bin on the bottom of the bin expanding horizontally
		if(rect.y + rect.height < bin.y + bin.height)
		{
			const Rect<int> new_bin = {bin.x, rect.y + rect.height, bin.width, bin.y + bin.height - (rect.y + rect.height)};

			if(new_bin.width >= minimum_dimension && new_bin.height >= minimum_dimension)
				bins.push_back(new_bin);
		}
		
		// If the left side of the rectangle is right of the left side of the bin 
		// inside it, then we can make a maximal bin on the left of the bin expanding vertically
		if(rect.x > bin.x)
		{
			const Rect<int> new_bin = {bin.x, bin.y, rect.x - bin.x, bin.height};

			if(new_bin.width >= minimum_dimension && new_bin.height >= minimum_dimension)
				bins.push_back(new_bin);
		}

		// If the right side of the rectangle is left of the left side of the bin
		// inside it, then we can make a maximal bin on the right of the bin expanding vertically
		if(rect.x + rect.width < bin.x + bin.width)
		{
			const Rect<int> new_bin = {rect.x + rect.width, bin.y, bin.x + bin.width - (rect.x + rect.width), bin.height};

			if(new_bin.width >= minimum_dimension && new_bin.height >= minimum_dimension)
				bins.push_back(new_bin);
		}
	}

	//-------------------------------------------------------------------------------------


	void insert_into_bin(Rect<int>& rect, const bool rotate, const int bin_index, std::vector<Rect<int>>& bins, const int minimum_dimension = 0)
	{
		// Update the location and orientation of the rect
		if(rotate) std::swap(rect.width, rect.height);
		rect.x = bins[bin_index].x;
		rect.y = bins[bin_index].y;

		// Every bin which intersects our rectangle needs to be  split into
		// 1-4 maximal bins surrounding the rectangle. These new bins will be 
		// added to a new_bins vector, instead of our normal bins one. Otherwise 
		// they will be checked for intersection when we know that they already do 
		// not intersect. This new vector will then be appended to the bins vector. 
		std::vector<Rect<int>> new_bins;
		for(int i = 0; i < bins.size(); i++)
		{
			const Rect<int> bin = bins[i];

			if(intersects(bin, rect))
			{
				split_bin(bin, rect, new_bins, minimum_dimension);
				swap_erase(bins, i);
				i--;
			}
		}
		bins.insert(bins.end(), new_bins.begin(), new_bins.end());

		// When we split the bins we will have made a bunch of
		// bins which are contained within other bins. We need
		// to remove this bins so that we only have maximal bins. 
		for(int i = 0; i < bins.size(); i++)
		{
			// If any dimension of bin i is smaller than 
			// the given minimum dimension we want to remove it.
			if(bins[i].width < minimum_dimension || bins[i].height < minimum_dimension)
			{
				swap_erase(bins, i);
				i--;
				continue;
			}

			for(int j = i + 1; j < bins.size(); j++)
			{

				// If any dimension of bin j is smaller than 
                // the given minimum dimension we want to remove it.
				if(bins[j].width < minimum_dimension || bins[j].height < minimum_dimension)
				{
					swap_erase(bins, j);
					j--;
					continue;
				}

				// if bin j contains i, we want to remove i 
				// and skip processing the rest of the j bins. 
				if(contains(bins[j], bins[i]))
				{
					swap_erase(bins, i);
					i--;
					break;
				}

				// If bin i contains j, we want to remove j
				if(contains(bins[i], bins[j]))
				{
					swap_erase(bins, j);
					j--;
				}

			}
		}

	}

	//-------------------------------------------------------------------------------------
	 
	bool max_rects_optimal(const glm::uvec2& size, const bool allow_rotation, const RectanglePackingHeuristic heuristic, std::vector<Rect<int>>& rectangles)
	{
		// We do not want to change the order of the given rectangles vector
		// so we will make a new vector which references each of the rectangles
		// Also since we are iterating over the rectangles, we want to determine
		// the total rectangle area, and the minimum rectangle dimension. 
		int minimum_dimension = std::numeric_limits<int>::max();
		unsigned required_rectangle_area = 0;

		std::vector<Rect<int>*> references;
		references.reserve(rectangles.size());
		for(auto& rect : rectangles)
		{
			// Accumulate the area
			required_rectangle_area += rect.width * rect.height;

			// Update minimum dimension
			if(int dim = std::min(rect.width, rect.height); dim < minimum_dimension)
				minimum_dimension = dim;

			references.push_back(&rect);
		}
		
		// If the total area of the rectangles exceeds the given area, exit out early
		if(required_rectangle_area > size.x * size.y)
			return false;

		std::vector<Rect<int>> bins;
		bins.push_back({0, 0, static_cast<int>(size.x), static_cast<int>(size.y)});

		// Insert rectangles until all have been added
		while(!references.empty())
		{
			// The best score is the best score for the bin and rect given
			// a heuristic. The best tiebreak is the score which is considered
			// when two sets of bins/rects have the same score as each other. 
			int best_tiebreak = std::numeric_limits<int>::max();
			int best_score = std::numeric_limits<int>::max();
			int best_rect_index = 0;
			int best_bin_index = 0;
			bool bin_found = false;
			bool rotated = false;

			// Since we are finding the optimal packing. We want
			// to find the global optimal combination of bin and rect
			for(int b = 0; b < bins.size(); b++)
			{
				const auto& bin = bins[b];

				for(int r = 0; r < references.size(); r++)
				{
					const auto& rect = *references[r];

					// If the rect fits in the bin, then calculate its score
					if(rect.width <= bin.width && rect.height <= bin.height)
					{
						bin_found = true;

						const auto [score, tiebreak] = calculate_score(bin, rect, heuristic);
						if(score < best_score || (score == best_score && tiebreak < best_tiebreak))
						{
							best_tiebreak = tiebreak;
							best_score = score;
							best_rect_index = r;
							best_bin_index = b;
							rotated = false;
						}
					}

					// Also check of the rect fits rotated, and if its a better fit
					if(allow_rotation && rect.height <= bin.width && rect.width <= bin.height)
					{
						bin_found = true;
						
						const auto [score, tiebreak] = calculate_score(bin, {rect.height, rect.width}, heuristic);
						if(score < best_score || (score == best_score && tiebreak < best_tiebreak))
						{
							best_tiebreak = tiebreak;
							best_score = score;
							best_rect_index = r;
							best_bin_index = b;
							rotated = true;
						}
					}
				}
			}

			// If no bin was found to place a rectangle into, then
			// we could not pack the rectangles into the given size
			if(!bin_found)
				return false;

			// Insert the rect into the bin. This will update
			// the rects location/orientation and split the 
			// bin as required, then prune non-maximal bins. 
			Rect<int>& chosen_rect = *references[best_rect_index];
			insert_into_bin(chosen_rect, rotated, best_bin_index, bins, minimum_dimension);
		
			// Remove the chosen rectangle from references so we dont pack it again
			swap_erase(references, best_rect_index);
		}

		return true;
	}
	
	//-------------------------------------------------------------------------------------
}