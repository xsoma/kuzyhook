#include "includes.h"
#include <cmath>
#include <math.h>       /* round, floor, ceil, trunc */

namespace render {
	Font menu;;
	Font menu_shade;;
	Font menu_b;;
	Font esp;;
	Font esp_small;;
	Font hud;;
	Font cs;;
	Font indicator;;
	Font indicators;;
	Font indicatorss;;
	Font logevent;;
	Font damage;;
	Font spectators;;
	Font menuicon;;
	Font arrows;;
	Font watermark;;
}




void render::init() {
	menu = Font(XOR("Verdana"), 12, FW_NORMAL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	menu_shade = Font(XOR("Verdana"), 12, FW_NORMAL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	menu_b = Font(XOR("Verdana"), 12, FW_NORMAL, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	esp = Font(XOR("Verdana"), 12, FW_NORMAL, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS);
	esp_small = Font(XOR("Small Fonts"), 8, FW_NORMAL, FONTFLAG_OUTLINE);
	hud = Font(XOR("Verdana"), 12, FW_NORMAL, FONTFLAG_ANTIALIAS);
	cs = Font(XOR("undefeated"), 13, FW_MEDIUM, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	logevent = Font(XOR("Verdana"), 12, FW_DONTCARE, FONTFLAG_DROPSHADOW);
	indicator = Font(XOR("Verdana"), 26, FW_BOLD, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	indicatorss = Font(XOR("Verdana"), 12, FW_MEDIUM, FONTFLAG_DROPSHADOW);
	damage = Font(XOR("Verdana"), 15, FW_BOLD, FONTFLAG_OUTLINE);
	menuicon = Font(XOR("menu_font"), 50, FW_NORMAL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	spectators = Font(XOR("Verdana"), 12, FW_NORMAL, FONTFLAG_DROPSHADOW);
	indicators = Font(XOR("Small Fonts"), 8, FW_NORMAL, FONTFLAG_OUTLINE);
	arrows = Font(XOR("Acta Symbols W95 Arrows"), 18, FW_NORMAL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
	watermark = Font(XOR("Verdana"), 12, FW_NORMAL, FONTFLAG_ANTIALIAS | FONTFLAG_DROPSHADOW);
}

bool render::WorldToScreen2(const vec3_t& world, vec3_t& screen) {
	float w;

	const VMatrix& matrix = g_csgo.m_engine->WorldToScreenMatrix();

	// check if it's in view first.
	// note - dex; w is below 0 when world position is around -90 / +90 from the player's camera on the y axis.
	w = matrix[3][0] * world.x + matrix[3][1] * world.y + matrix[3][2] * world.z + matrix[3][3];
	if (w < 0.001f)
		return false;

	// calculate x and y.
	screen.x = matrix[0][0] * world.x + matrix[0][1] * world.y + matrix[0][2] * world.z + matrix[0][3];
	screen.y = matrix[1][0] * world.x + matrix[1][1] * world.y + matrix[1][2] * world.z + matrix[1][3];

	screen /= w;

	// calculate screen position.
	screen.x = (g_cl.m_width / 2) + (screen.x * g_cl.m_width) / 2;
	screen.y = (g_cl.m_height / 2) - (screen.y * g_cl.m_height) / 2;

	return true;
}


#define ZERO vec3_t(0.0f, 0.0f, 0.0f)

void render::Draw3DFilledCircle(const vec3_t& origin, float radius, Color color) // OUTLINE 
{
	auto prevScreenPos = ZERO; //-V656
	auto step = math::pi * 2.0f / 72.0f;

	auto screenPos = ZERO;
	auto screen = ZERO;

	if (!render::WorldToScreen2(origin, screen))
		return;

	for (auto rotation = 0.0f; rotation <= math::pi * 2.0f; rotation += step) //-V1034
	{
		vec3_t pos(radius * cos(rotation) + origin.x, radius * sin(rotation) + origin.y, origin.z);

		if (render::WorldToScreen2(pos, screenPos))
		{
			if (!prevScreenPos.IsZero() && prevScreenPos.IsValid() && screenPos.IsValid() && prevScreenPos != screenPos)
			{
				render::DrawLine(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y, color, true);
			}

			prevScreenPos = screenPos;
		}
	}
}


void render::DrawLine(int x0, int y0, int x1, int y1, Color col, bool shadow)
{
	g_csgo.m_surface->DrawSetColor(col);
	g_csgo.m_surface->DrawLine(x0, y0, x1, y1);
}


void render::DrawFilledCircle(int x, int y, int radius, int segments, Color color) {
	std::vector< Vertex > vertices;

	float step = math::pi * 2.0f / segments;

	for (float a = 0; a < (math::pi * 2.0f); a += step)
		vertices.emplace_back(radius * cosf(a) + x, radius * sinf(a) + y);

	TexturedPolygon(vertices.size(), vertices, color);
}

void render::TexturedPolygon(int n, std::vector< Vertex > vertice, Color color) {
	static int texture_id = g_csgo.m_surface->CreateNewTextureID(true); // 
	static unsigned char buf[4] = { 255, 255, 255, 255 };
	g_csgo.m_surface->DrawSetTextureRGBA(texture_id, buf, 1, 1); //
	g_csgo.m_surface->DrawSetColor(color); //
	g_csgo.m_surface->DrawSetTexture(texture_id); //
	g_csgo.m_surface->DrawTexturedPolygon(n, vertice.data()); //
}

void render::gradient1337(int x, int y, int w, int h, Color color1, Color color2) {
	g_csgo.m_surface->DrawSetColor(color1);
	g_csgo.m_surface->DrawFilledRectFade(x, y, x + w, y + h, color1.a(), 0, true);

	g_csgo.m_surface->DrawSetColor(color2);
	g_csgo.m_surface->DrawFilledRectFade(x, y, x + w, y + h, 0, color2.a(), true);
}

bool render::WorldToScreen(const vec3_t& world, vec2_t& screen) {
	float w;

	const VMatrix& matrix = g_csgo.m_engine->WorldToScreenMatrix();

	// check if it's in view first.
	// note - dex; w is below 0 when world position is around -90 / +90 from the player's camera on the y axis.
	w = matrix[3][0] * world.x + matrix[3][1] * world.y + matrix[3][2] * world.z + matrix[3][3];
	if (w < 0.001f)
		return false;

	// calculate x and y.
	screen.x = matrix[0][0] * world.x + matrix[0][1] * world.y + matrix[0][2] * world.z + matrix[0][3];
	screen.y = matrix[1][0] * world.x + matrix[1][1] * world.y + matrix[1][2] * world.z + matrix[1][3];

	screen /= w;

	// calculate screen position.
	screen.x = (g_cl.m_width / 2) + (screen.x * g_cl.m_width) / 2;
	screen.y = (g_cl.m_height / 2) - (screen.y * g_cl.m_height) / 2;

	return true;
}

void render::line(vec2_t v0, vec2_t v1, Color color) {
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawLine(v0.x, v0.y, v1.x, v1.y);
}

void render::line(int x0, int y0, int x1, int y1, Color color) {
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawLine(x0, y0, x1, y1);
}

#define ZERO vec3_t(0.0f, 0.0f, 0.0f)

void render::Draw3DRainbowCircle(const vec3_t& origin, float radius, Color color)
{
	auto hue_offset = 0.0f;
	auto prevScreenPos = ZERO; //-V656

	auto step = math::pi * 2.0f / 72.0f;
	auto screenPos = ZERO;

	for (auto rotation = 0.0f; rotation <= math::pi * 2.0f; rotation += step) //-V1034
	{
		vec3_t pos(radius * cos(rotation) + origin.x, radius * sin(rotation) + origin.y, origin.z);

		CGameTrace trace;
		CTraceFilter filter;

		filter.skip_entity = g_cl.m_local;

		g_csgo.m_engine_trace->TraceRay(Ray(origin, pos), MASK_SHOT_BRUSHONLY, &filter, &trace);

		if (render::WorldToScreen2(trace.m_endpos, screenPos))
		{
			if (!prevScreenPos.IsZero())
			{
				render::DrawLine(prevScreenPos.x, prevScreenPos.y, screenPos.x, screenPos.y, color, true);
			}

			prevScreenPos = screenPos;
		}
	}
}

void render::DrawRectGradientHorizontal(int x, int y, int width, int height, Color color1, Color color2)
{
	float flDifferenceR = (float)(color2.r() - color1.r()) / (float)width;
	float flDifferenceG = (float)(color2.g() - color1.g()) / (float)width;
	float flDifferenceB = (float)(color2.b() - color1.b()) / (float)width;
	float flDifferenceA = (float)(color2.a() - color1.a()) / (float)width;

	for (float i = 0.f; i < width; i++)
	{
		Color colGradient = Color(color1.r() + (flDifferenceR * i), color1.g() + (flDifferenceG * i), color1.b() + (flDifferenceB * i), color1.a() + (flDifferenceA * i));
		render::rect(x + i, y, 1, height, colGradient);
	}
}

void render::rect(int x, int y, int w, int h, Color color) {
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void render::rect_filled(int x, int y, int w, int h, Color color) {
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawFilledRect(x, y, x + w, y + h);
}

void render::rect_filled_fade(int x, int y, int w, int h, Color color, int a1, int a2) {
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawFilledRectFade(x, y, x + w, y + h, a1, a2, false);
}

void render::rect_outlined(int x, int y, int w, int h, Color color, Color color2) {
	rect(x, y, w, h, color);
	rect(x - 1, y - 1, w + 2, h + 2, color2);
	rect(x + 1, y + 1, w - 2, h - 2, color2);
}


void render::arccircle(int x, int y, int r1, int r2, int s, int d, Color color) {
	g_csgo.m_surface->DrawSetColor(color);
	for (int i = s; i < s + d; i++) {

		float rad = i * math::pi / 180;

		g_csgo.m_surface->DrawLine(x + cos(rad) * r1, y + sin(rad) * r1, x + cos(rad) * r2, y + sin(rad) * r2);
	}
}

void render::drawCircle(int x, int y, int angle, Color color) {
	float granularity = 2 * math::pi / 700;
	float step = 2 * math::pi / 100;
	float inner = 6;

	for (int radius = 8; inner < -1; radius--) {
		for (int angle = 0; angle * step < -1; granularity++) {
			int px = round(radius * cos(angle));
			int py = round(radius * sin(angle));

			g_csgo.m_surface->DrawLine(px + x, py + y, px + 1 + x, py + 1 + y);
		}

	}

}

void DrawColoredCircle(int centerx, int centery, float radius, int r, int g, int b, int a)
{
	typedef void(__thiscall* OriginalFn)(void*, int, int, float, int, int, int, int);
	
}

void DrawFilledCircle(int centerx, int centery, float radius, int r, int g, int b, int a)
{
	for (int i = 0; i < radius; i++)
	{
		DrawColoredCircle(centerx, centery, i, r, g, b, a);
	}
}
 

void render::draw_arc(int x, int y, int radius, int start_angle, int percent, int thickness, Color color) {
	float precision = (2 * math::pi) / 30;
	float step = math::pi / 180;
	float inner = radius - thickness;
	float end_angle = (start_angle + percent) * step;
	float start_angle1337 = (start_angle * math::pi) / 180;

	for (; radius > inner; --radius) {
		for (float angle = start_angle1337; angle < end_angle; angle += precision) {
			float cx = round(x + radius * cos(angle));
			float cy = round(y + radius * sin(angle));

			float cx2 = round(x + radius * cos(angle + precision));
			float cy2 = round(y + radius * sin(angle + precision));

			g_csgo.m_surface->DrawSetColor(color);
			g_csgo.m_surface->DrawLine(cx, cy, cx2, cy2);
		}
	}
}

void render::circle(int x, int y, int radius, int segments, Color color) {
	static int texture = g_csgo.m_surface->CreateNewTextureID(true);

	g_csgo.m_surface->DrawSetTextureRGBA(texture, &colors::white, 1, 1);
	g_csgo.m_surface->DrawSetColor(color);
	g_csgo.m_surface->DrawSetTexture(texture);

	std::vector< Vertex > vertices{};

	float step = math::pi_2 / segments;
	for (float i{ 0.f }; i < math::pi_2; i += step)
		vertices.emplace_back(vec2_t{ x + (radius * std::cos(i)), y + (radius * std::sin(i)) });

	g_csgo.m_surface->DrawTexturedPolygon(vertices.size(), vertices.data());
}
void render::gradient(int x, int y, int w, int h, Color color1, Color color2) {
	g_csgo.m_surface->DrawSetColor(color1);
	g_csgo.m_surface->DrawFilledRectFade(x, y, x + w, y + h, color1.a(), 0, false);

	g_csgo.m_surface->DrawSetColor(color2);
	g_csgo.m_surface->DrawFilledRectFade(x, y, x + w, y + h, 0, color2.a(), false);
}

void render::sphere(vec3_t origin, float radius, float angle, float scale, Color color) {
	std::vector< Vertex > vertices{};

	// compute angle step for input radius and precision.
	float step = (1.f / radius) + math::deg_to_rad(angle);

	for (float lat{}; lat < (math::pi * scale); lat += step) {
		// reset.
		vertices.clear();

		for (float lon{}; lon < math::pi_2; lon += step) {
			vec3_t point{
				origin.x + (radius * std::sin(lat) * std::cos(lon)),
				origin.y + (radius * std::sin(lat) * std::sin(lon)),
				origin.z + (radius * std::cos(lat))
			};

			vec2_t screen;
			if (WorldToScreen(point, screen))
				vertices.emplace_back(screen);
		}

		if (vertices.empty())
			continue;

		g_csgo.m_surface->DrawSetColor(color);
		g_csgo.m_surface->DrawTexturedPolyLine(vertices.size(), vertices.data());
	}
}

Vertex render::RotateVertex(const vec2_t& p, const Vertex& v, float angle) {
	// convert theta angle to sine and cosine representations.
	float c = std::cos(math::deg_to_rad(angle));
	float s = std::sin(math::deg_to_rad(angle));

	return {
		p.x + (v.m_pos.x - p.x) * c - (v.m_pos.y - p.y) * s,
		p.y + (v.m_pos.x - p.x) * s + (v.m_pos.y - p.y) * c
	};
}

void render::Font::string(int x, int y, Color color, const std::string& text, StringFlags_t flags /*= render::ALIGN_LEFT */) {
	wstring(x, y, color, util::MultiByteToWide(text), flags);
}

void render::Font::string(int x, int y, Color color, const std::stringstream& text, StringFlags_t flags /*= render::ALIGN_LEFT */) {
	wstring(x, y, color, util::MultiByteToWide(text.str()), flags);
}

void render::Font::wstring(int x, int y, Color color, const std::wstring& text, StringFlags_t flags /*= render::ALIGN_LEFT */) {
	int w, h;

	g_csgo.m_surface->GetTextSize(m_handle, text.c_str(), w, h);
	g_csgo.m_surface->DrawSetTextFont(m_handle);
	g_csgo.m_surface->DrawSetTextColor(color);

	if (flags & ALIGN_RIGHT)
		x -= w;
	if (flags & render::ALIGN_CENTER)
		x -= w / 2;

	g_csgo.m_surface->DrawSetTextPos(x, y);
	g_csgo.m_surface->DrawPrintText(text.c_str(), (int)text.size());
}

render::FontSize_t render::Font::size(const std::string& text) {
	return wsize(util::MultiByteToWide(text));
}

render::FontSize_t render::Font::wsize(const std::wstring& text) {
	FontSize_t res;
	g_csgo.m_surface->GetTextSize(m_handle, text.data(), res.m_width, res.m_height);
	return res;
}
