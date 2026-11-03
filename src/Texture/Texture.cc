#include <moot/Texture/Texture.hh>
#include <moot/Texture/Layer.hh>
#include <moot/procgen/Field.hh>
#include <moot/procgen/hash.hh>
#include <bit>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <ranges>
#include <vector>

Texture::Texture(float worldSize, float texelsPerUnit, unsigned seed, std::span<const Layer> layers)
{
	assert(worldSize > 0 && texelsPerUnit > 0);
	const auto size = static_cast<unsigned>(std::lround(worldSize * texelsPerUnit));
	m_texelsPerUnit = size / worldSize;
	Field tile(size, 1.f);

	for (const unsigned i : std::views::iota(0u, layers.size()))
		layers[i].paint(&tile, texelsPerUnit, deriveSeed(seed, i));

	std::vector<std::uint32_t> pixels(tile.values().size());
	std::uint32_t* pixel = pixels.data();
	for (const float lightness : tile.values())
	{
		assert(lightness >= 0 && lightness <= 1);
		const auto channel = std::uint32_t(lightness * 255 + 0.5f);
		static_assert(std::endian::native == std::endian::little);
		*pixel++ = channel | channel << 8 | channel << 16 | 0xff000000u;
	}

	if (!resize({size, size}))
		assert(false);
	update(reinterpret_cast<const std::uint8_t*>(pixels.data()));

	setRepeated(true);
	setSmooth(true);
	if (!generateMipmap())
		assert(false);
}
