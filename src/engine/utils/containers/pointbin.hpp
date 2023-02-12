#pragma once

#include "../../math/random.hpp"
#include <engine/utils/containers/pointbin.hpp>
#include <glm/glm.hpp>
#include <vector>

namespace utils {

	// N-dimensional fixed-size bin structure to store elements addressed by points.
	template<typename T, int Dim, typename FloatT = float>
	class PointBin
	{
	public:
		using VecT = glm::vec<Dim, FloatT, glm::defaultp>;
		using IVecT = glm::vec<Dim, int32_t, glm::defaultp>;
		using Bin = std::vector<std::pair<VecT, T>>;

		/// @brief Construct a box split into a number of rectangular bins.
		/// @param _size Total size of the box.
		/// @param _partition The number of bins for each dimension.
		/// @param _origin Absolute position of the box corresponding to bin (0,0).
		PointBin(const VecT& _size, const IVecT& _partition, const VecT& _origin = VecT{});

		/// @brief Add an element to the bin.
		void add(const VecT& _position, const T& _el);

		/// @brief Invoke a function on each point that is in a bin within the radius.
		/// @param _pred Functor called on each point with signature
		///		void operator()(const VecT&, T& el)
		template<typename Pred>
		void iterateNeighbours(const VecT& _position, FloatT r, Pred _pred);

		// Access a bin with a multi-dimensional index.
		const Bin& operator[](const IVecT& idx) const { return m_bins[idx.x + idx.y * m_partition.x]; }
		Bin& operator[](const IVecT& idx) { return m_bins[idx.x + idx.y * m_partition.x]; }
	private:
		template<int D, typename Pred>
		void iterateNeighboursImpl(const IVecT& _start, const IVecT& _end, IVecT& cur, Pred _pred);

		IVecT getBinIdx(const VecT& _position) const noexcept
		{
			const VecT pos = _position - m_origin;
			const VecT relPos = clamp(pos / m_size, VecT(0.f), VecT(0.999f));
			return relPos * VecT(m_partition);
		}

		VecT m_size;
		IVecT m_partition;
		std::vector<Bin> m_bins;
		VecT m_origin;
	};


	// ********************************************************************* //
	// implementation
	// ********************************************************************* //
	template<typename T, int Dim, typename FloatT>
	PointBin<T, Dim, FloatT>::PointBin(const VecT& _size, const IVecT& _partition, const VecT& _origin)
		: m_size(_size)
		, m_partition(_partition)
		, m_bins(_partition.x* _partition.y)
		, m_origin(_origin)
	{}

	template<typename T, int Dim, typename FloatT>
	void PointBin<T,Dim,FloatT>::add(const VecT& _position, const T& _el)
	{
		Bin& bin = (*this)[getBinIdx(_position)];
		bin.emplace_back(_position, _el);
	}

	template<typename T, int Dim, typename FloatT>
	template<typename Pred>
	void PointBin<T, Dim, FloatT>::iterateNeighbours(const VecT& _position, FloatT r, Pred _pred)
	{
		const VecT pos = _position - m_origin;
		const IVecT start = getBinIdx(pos - r);
		const IVecT end = getBinIdx(pos + r);

		IVecT cur{};
		iterateNeighboursImpl<Dim - 1>(start, end, cur, _pred);
	}

	template<typename T, int Dim, typename FloatT>
	template<int D, typename Pred>
	void PointBin<T, Dim, FloatT>::iterateNeighboursImpl(const IVecT& _start, const IVecT& _end, IVecT& cur, Pred _pred)
	{
		for (int i = _start[D]; i < _end[D]; ++i)
		{
			cur[D] = i;
			if constexpr (D == 0)
			{
				for (auto& [pos, el] : (*this)[cur])
				{
					_pred(pos, el);
				}
			}
			else
				iterateNeighboursImpl<D - 1>(_start, _end, cur, _pred);
		}
	}
}