#ifndef FASTGLTF_MATH_HPP
namespace fastgltf { class Asset;
#if defined(FASTGLTF_USE_64BIT_FLOAT) && FASTGLTF_USE_64BIT_FLOAT
    using num = double;
#else
    using num = float;
#endif
    namespace math {
        template <typename T>
        class mat; 
        template <size_t Rows, size_t Cols>
        using fmat = mat<float>; 
        using fmat4x4 = fmat<4, 4>;
        using fmat3x3 = fmat<3, 3>;

        template <typename T>
        class vec; 
        template <size_t Cols>
        using fvec = vec<float>; 
        using fvec2 = fvec<2>;
        using fvec3 = fvec<3>;
        using fvec4 = fvec<4>;
		using nvec2 = vec<num, 2>;
		using nvec3 = vec<num, 3>;
		using nvec4 = vec<num, 4>;
    }
    struct Node;
}
#endif