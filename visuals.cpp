#include "includes.h"

Visuals g_visuals{ };;

void Visuals::ModulateWorld() {
	std::vector< IMaterial* > world, props;

	// iterate material handles.
	for (uint16_t h{ g_csgo.m_material_system->FirstMaterial() }; h != g_csgo.m_material_system->InvalidMaterial(); h = g_csgo.m_material_system->NextMaterial(h)) {
		// get material from handle.
		IMaterial* mat = g_csgo.m_material_system->GetMaterial(h);
		if (!mat)
			continue;

		// store world materials.
		if (FNV1a::get(mat->GetTextureGroupName()) == HASH("World textures"))
			world.push_back(mat);

		// store props.
		else if (FNV1a::get(mat->GetTextureGroupName()) == HASH("StaticProp textures"))
			props.push_back(mat);
	}

	// night
	const float darkness = g_menu.main.visuals.night_darkness.get() / 100.f;

	if (g_menu.main.visuals.world.get() == 1) {
		for (const auto& w : world)
			w->ColorModulate(darkness, darkness, darkness);


		


		    

		// IsUsingStaticPropDebugModes my nigga
		if (g_csgo.r_DrawSpecificStaticProp->GetInt() != 0) {
			g_csgo.r_DrawSpecificStaticProp->SetValue(0);
		}

		for (const auto& p : props)
			p->ColorModulate(0.5f, 0.5f, 0.5f);

		//game::SetSkybox(XOR("sky_csgo_night02"));
	}

	// disable night.
	else {
		for (const auto& w : world)
			w->ColorModulate(1.f, 1.f, 1.f);

		// restore r_DrawSpecificStaticProp.
		if (g_csgo.r_DrawSpecificStaticProp->GetInt() != -1) {
			g_csgo.r_DrawSpecificStaticProp->SetValue(-1);
		}

		for (const auto& p : props)
			p->ColorModulate(1.f, 1.f, 1.f);
	}

	// transparent props.
	if (g_menu.main.visuals.transparent_props.get()) {

		// IsUsingStaticPropDebugModes my nigga
		if (g_csgo.r_DrawSpecificStaticProp->GetInt() != 0) {
			g_csgo.r_DrawSpecificStaticProp->SetValue(0);
		}

		float alpha = g_menu.main.visuals.transparent_props_amount.get() / 100;
		for (const auto& p : props)
			p->AlphaModulate(alpha);
	}

	// disable transparent props.
	else {

		// restore r_DrawSpecificStaticProp.
		if (g_csgo.r_DrawSpecificStaticProp->GetInt() != -1) {
			g_csgo.r_DrawSpecificStaticProp->SetValue(-1);
		}

		for (const auto& p : props)
			p->AlphaModulate(1.0f);
	}
}


void Visuals::DrawProjectile(Weapon* ent) {
	vec2_t screen;
	vec3_t origin = ent->GetAbsOrigin();
	if (!render::WorldToScreen(origin, screen))
		return;

	if (!g_csgo.m_engine->IsInGame() && !g_csgo.m_engine->IsConnected())
		return;

	Color col = g_menu.main.visuals.proj_color.get();
	col.a() = 0xb4;

	if (ent->is(HASH("CHostage"))) {
		std::string distance;
		int dist = (((ent->m_vecOrigin() - g_cl.m_local->m_vecOrigin()).length_sqr()) * 0.0625) * 0.001;
		if (dist < 150) {
			render::esp_small.string(screen.x, screen.y, colors::light_blue, XOR("HOSTAGE"), render::ALIGN_CENTER);
			//render::esp_small.string(screen.x, screen.y - 10, colors::light_blue, distance, render::ALIGN_CENTER);
		}
	}

	// draw decoy.
	if (ent->is(HASH("CDecoyProjectile"))) {
		render::esp_small.string(screen.x, screen.y, col, XOR("DECOY"), render::ALIGN_CENTER);
	}

	// draw molotov.
	else if (ent->is(HASH("CMolotovProjectile"))) {
		render::esp_small.string(screen.x, screen.y - 15, col, XOR("MOLLY"), render::ALIGN_CENTER);
		//render::draw_arc(screen.x, screen.y, 6, 0, 360, 1.f, { 255,255,255,255 });
	}

	else if (ent->is(HASH("CBaseCSGrenadeProjectile"))) {
		const model_t* model = ent->GetModel();

		if (model) {
			// grab modelname.
			std::string name{ ent->GetModel()->m_name };

			if (name.find(XOR("flashbang")) != std::string::npos)
				render::esp_small.string(screen.x, screen.y, col, XOR("FLASH"), render::ALIGN_CENTER);

			else if (name.find(XOR("fraggrenade")) != std::string::npos) {
				render::esp_small.string(screen.x, screen.y - 15, col, XOR("GRENADE"), render::ALIGN_CENTER);
				//render::draw_arc(screen.x, screen.y, 6, 0, 360, 1.f, { 255,255,255,255 });
			}
		} 
	}

	auto dist_world = g_cl.m_local->m_vecOrigin().dist_to(origin);

	//const double spawn_time_fire = *(float*)(uintptr_t(ent) + 0x20);
	//const float spawn_time_smoke = game::TICKS_TO_TIME(ent->m_nSmokeEffectTickBegin());

	static auto size = vec2_t(25.0f, 5.0f);

	auto inferno = reinterpret_cast<c_cs_inferno*>(ent);
	auto spawn_time = inferno->get_entity_spawn_time();
	auto factor = (spawn_time + c_cs_inferno::expire_time - g_csgo.m_globals->m_curtime) / c_cs_inferno::expire_time;

	auto smoke = reinterpret_cast<c_cs_inferno*>(ent);
	static constexpr float max_time = 17.5;
	const auto actual = (((spawn_time + max_time) - g_csgo.m_globals->m_curtime) / max_time);

	if (actual <= 0)
		return;

	// find classes.
	else if (ent->is(HASH("CInferno"))) {
		//if (dist_world > 400.f) 
		//render::esp_small.string(screen.x, screen.y + 3, col, XOR("FIRE"), render::ALIGN_CENTER);
		//else
			//render::DrawString(F::ESPInfo, screen.x, screen.y + 8.0, Color(255, 255, 255, 255), FONT_CENTER, "FIRE - %.1f", (spawn_time_fire + 7.031) - g_csgo.m_globals->m_curtime);

		if (dist_world > 400.f) {
			col.a() *= std::clamp((750.f - (dist_world - 400.f)) / 750.f, 0.f, 1.f);
		}

		
		render::Draw3DRainbowCircle(origin, 165.f, Color(165, 0, 0, col.a()));
		

		// circle timer bar
		if (g_menu.main.visuals.proj_range.get(0)) {
			render::DrawFilledCircle(screen.x, screen.y - 7, 22, 180, Color(26, 26, 30, col.a()));
			render::DrawFilledCircle(screen.x, screen.y - 7, 22, 180, Color(26, 26, 30, col.a()));
			render::draw_arc(screen.x, screen.y - 7, 22, 0, 360 * factor, 1, Color(255, 255, 255, col.a()));
			render::cs.string(screen.x, screen.y - 21, Color(255, 255, 255, col.a()), XOR("l"), render::ALIGN_CENTER);

		}

		render::esp_small.string(screen.x, screen.y - 3, { 255,255,255,200 }, XOR("MOLLY"), render::ALIGN_CENTER);

		// render time of fire like bar(like drop weap ammo...)
		//if (g_menu.main.visuals.proj_range.get(0)) {
		//	render::rect_filled(screen.x - size.x * 0.5f, screen.y - size.y * 0.5f - 1.0f, size.x, size.y, Color(80, 80, 80, col.a()));
		//	render::rect_filled(screen.x - size.x * 0.5f + 1.0f, screen.y - size.y * 0.5f, (size.x - 4.0f) * factor, size.y - 2.0f, Color(g_menu.main.visuals.proj_range_color.get().r(), g_menu.main.visuals.proj_range_color.get().g(), g_menu.main.visuals.proj_range_color.get().b(), col.a()));
		//	render::rect(screen.x - size.x * 0.5f, screen.y - size.y * 0.5f, size.x, size.y, Color(7, 7, 7, col.a()));
		// }
	}

	else if (ent->is(HASH("CSmokeGrenadeProjectile"))) {

		//if (dist_world > 400.f) 
		//render::esp_small.string(screen.x, screen.y + 3, col, XOR("SMOKE"), render::ALIGN_CENTER);
		//else
			//render::DrawString(F::ESPInfo, screen.x, screen.y + 8.0, Color(255, 255, 255, 255), FONT_CENTER, "SMOKE - %.1f", (spawn_time_smoke + 18.04125) - g_csgo.m_globals->m_curtime);

		if (dist_world > 400.f) {
			col.a() *= std::clamp((750.f - (dist_world - 400.f)) / 750.f, 0.f, 1.f);
		}

		
			render::Draw3DFilledCircle(origin, 144, Color(66, 133, 180, col.a()));
		

		// circle timer bar
		if (g_menu.main.visuals.proj_range.get(1)) {
			render::DrawFilledCircle(screen.x, screen.y - 7, 22, 180, Color(26, 26, 30, col.a()));
			render::DrawFilledCircle(screen.x, screen.y - 7, 22, 180, Color(26, 26, 30, col.a()));
			render::draw_arc(screen.x, screen.y - 7, 22, 0, 360 * actual, 1, Color(255, 255, 255, col.a()));
			render::cs.string(screen.x, screen.y - 21, Color(255, 255, 255, col.a()), XOR("k"), render::ALIGN_CENTER);

		}

		render::esp_small.string(screen.x, screen.y - 3, { 255,255,255,200 }, XOR("SMOKE"), render::ALIGN_CENTER);

		// render time of smoke like bar (like drop weap ammo...)
		//if (actual > 0.f && actual < 18.f && g_menu.main.visuals.proj_range.get(1)) {
		//	render::rect_filled(screen.x - size.x * 0.5f, screen.y - size.y * 0.5f - 1.0f, size.x, size.y, Color(80, 80, 80, col.a()));
		//	render::rect_filled(screen.x - size.x * 0.5f + 1.0f, screen.y - size.y * 0.5f, (size.x - 4.0f) * actual, size.y - 2.0f, Color(g_menu.main.visuals.proj_range_color.get().r(), g_menu.main.visuals.proj_range_color.get().g(), g_menu.main.visuals.proj_range_color.get().b(), col.a()));
		//	render::rect(screen.x - size.x * 0.5f, screen.y - size.y * 0.5f, size.x, size.y, Color(7, 7, 7, col.a()));
		// }
	}
}

void Visuals::ThirdpersonThink() {
	ang_t                          offset;
	vec3_t                         origin, forward;
	static CTraceFilterSimple_game filter{ };
	CGameTrace                     tr;

	// for whatever reason overrideview also gets called from the main menu.
	if (!g_csgo.m_engine->IsInGame())
		return;

	// check if we have a local player and he is alive.
	bool alive = g_cl.m_local && g_cl.m_local->alive();

	// camera should be in thirdperson.
	if (m_thirdperson) {

		// if alive and not in thirdperson already switch to thirdperson.
		if (alive && !g_csgo.m_input->CAM_IsThirdPerson())
			g_csgo.m_input->CAM_ToThirdPerson();

		// if dead and spectating in firstperson switch to thirdperson.
		else if (g_cl.m_local->m_iObserverMode() == 4) {

			// if in thirdperson, switch to firstperson.
			// we need to disable thirdperson to spectate properly.
			if (g_csgo.m_input->CAM_IsThirdPerson()) {
				g_csgo.m_input->CAM_ToFirstPerson();
				g_csgo.m_input->m_camera_offset.z = 0.f;
			}

			g_cl.m_local->m_iObserverMode() = 5;
		}
	}

	// camera should be in firstperson.
	else if (g_csgo.m_input->CAM_IsThirdPerson()) {
		g_csgo.m_input->CAM_ToFirstPerson();
		g_csgo.m_input->m_camera_offset.z = 0.f;
	}

	// if after all of this we are still in thirdperson.
	if (g_csgo.m_input->CAM_IsThirdPerson()) {
		// get camera angles.
		g_csgo.m_engine->GetViewAngles(offset);

		// get our viewangle's forward directional vector.
		math::AngleVectors(offset, &forward);

		// cam_idealdist convar.
		offset.z = g_menu.main.visuals.thirdperson_distance.get();

		// start pos.
		origin = g_cl.m_shoot_pos;

		// setup trace filter and trace.
		filter.SetPassEntity(g_cl.m_local);

		g_csgo.m_engine_trace->TraceRay(
			Ray(origin, origin - (forward * offset.z), { -16.f, -16.f, -16.f }, { 16.f, 16.f, 16.f }),
			MASK_NPCWORLDSTATIC,
			(ITraceFilter*)&filter,
			&tr
		);

		// adapt distance to travel time.
		math::clamp(tr.m_fraction, 0.f, 1.f);
		offset.z *= tr.m_fraction;

		// override camera angles.
		g_csgo.m_input->m_camera_offset = { offset.x, offset.y, offset.z };
	}
}



// meme...
void Visuals::IndicateAngles()
{
	if (!g_csgo.m_engine->IsInGame() && !g_csgo.m_engine->IsConnected())
		return;

	if (!g_menu.main.antiaim.draw_angles.get())
		return;

	if (!	g_csgo.m_input->CAM_IsThirdPerson())
		return;

	if (!g_cl.m_local || g_cl.m_local->m_iHealth() < 1)
		return;

	const auto &pos = g_cl.m_local->GetRenderOrigin();
	vec2_t tmp;

	if (render::WorldToScreen(pos, tmp))
	{
		vec2_t draw_tmp;
		const vec3_t real_pos(50.f * std::cos(math::deg_to_rad(g_cl.m_radar.y)) + pos.x, 50.f * sin(math::deg_to_rad(g_cl.m_radar.y)) + pos.y, pos.z);

		if (render::WorldToScreen(real_pos, draw_tmp))
		{
			render::line(tmp.x, tmp.y, draw_tmp.x, draw_tmp.y, { 0, 255, 0, 255 });
			render::esp_small.string(draw_tmp.x, draw_tmp.y, { 0, 255, 0, 255 }, "FAKE", render::ALIGN_LEFT);
		}

		if (g_menu.main.antiaim.fake_yaw.get())
		{
			const vec3_t fake_pos(50.f * cos(math::deg_to_rad(g_cl.m_angle.y)) + pos.x, 50.f * sin(math::deg_to_rad(g_cl.m_angle.y)) + pos.y, pos.z);

			if (render::WorldToScreen(fake_pos, draw_tmp))
			{
				render::line(tmp.x, tmp.y, draw_tmp.x, draw_tmp.y, { 255, 0, 0, 255 });
				render::esp_small.string(draw_tmp.x, draw_tmp.y, { 255, 0, 0, 255 }, "REAL", render::ALIGN_LEFT);

			}
		}
	}
}






void Visuals::Hitmarker() {

	static auto aspect = g_csgo.m_cvar->FindVar(HASH("r_aspectratio"));
	aspect->SetValue(g_menu.main.misc.aspect.get());



	static auto cross = g_csgo.m_cvar->FindVar(HASH("weapon_debug_spread_show"));
	cross->SetValue(g_menu.main.visuals.force_xhair.get() && !g_cl.m_local->m_bIsScoped() ? 3 : 0);

	if (!g_menu.main.misc.hitmarker.get())
		return;

	if (g_csgo.m_globals->m_curtime > m_hit_end)
		return;

	if (m_hit_duration <= 0.f)
		return;

	float complete = (g_csgo.m_globals->m_curtime - m_hit_start) / m_hit_duration;
	int x = g_cl.m_width,
		y = g_cl.m_height,
		alpha = (1.f - complete) * 240;

	constexpr int line{ 6 };


	render::rect_filled(x / 2 + 6, y / 2 + 6, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 + 7, y / 2 + 7, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 + 8, y / 2 + 8, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 + 9, y / 2 + 9, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 + 10, y / 2 + 10, 1, 1, { 200, 200, 200, alpha });

	render::rect_filled(x / 2 - 6, y / 2 - 6, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 - 7, y / 2 - 7, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 - 8, y / 2 - 8, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 - 9, y / 2 - 9, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 - 10, y / 2 - 10, 1, 1, { 200, 200, 200, alpha });

	render::rect_filled(x / 2 - 6, y / 2 + 6, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 - 7, y / 2 + 7, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 - 8, y / 2 + 8, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 - 9, y / 2 + 9, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 - 10, y / 2 + 10, 1, 1, { 200, 200, 200, alpha });

	render::rect_filled(x / 2 + 6, y / 2 - 6, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 + 7, y / 2 - 7, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 + 8, y / 2 - 8, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 + 9, y / 2 - 9, 1, 1, { 200, 200, 200, alpha });
	render::rect_filled(x / 2 + 10, y / 2 - 10, 1, 1, { 200, 200, 200, alpha });


	std::string out = tfm::format(XOR("%i\n"), g_shots.iHitDmg);

	for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
		Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);

		if (render::WorldToScreen(g_shots.iPlayermins, g_shots.iPlayerbottom) && render::WorldToScreen(g_shots.iPlayermaxs, g_shots.iPlayertop))
		{
			// get the esp box info >_<
			Rect box;
			box.h = g_shots.iPlayerbottom.y - g_shots.iPlayertop.y;
			box.w = box.h / 2.f;
			box.x = g_shots.iPlayerbottom.x - (box.w / 2.f);
			box.y = g_shots.iPlayerbottom.y - box.h;




		}
	}
}







void Visuals::NoSmoke() {
	if (!g_menu.main.visuals.nosmoke.get())
		return;
	//colado https://www.unknowncheats.me/forum/counterstrike-global-offensive/262635-epic-wireframe-smoke.html
	std::vector<const char*> vistasmoke_mats =
	{
			"particle/vistasmokev1/vistasmokev1_fire",
			"particle/vistasmokev1/vistasmokev1_smokegrenade",
			"particle/vistasmokev1/vistasmokev1_emods",
			"particle/vistasmokev1/vistasmokev1_emods_impactdust",
	};



	for (auto mat_s : vistasmoke_mats)
	{
		IMaterial* mat = g_csgo.m_material_system->FindMaterial(mat_s, XOR("Other textures"));
		mat->SetFlag(MATERIAL_VAR_WIREFRAME, true);
	}
}


void Visuals::NoMolly() {
	if (!g_menu.main.visuals.nosmoke.get())
		return;
	//colado https://www.unknowncheats.me/forum/counterstrike-global-offensive/262635-epic-wireframe-smoke.html
	std::vector<const char*> vistasmoke_mats =
	{
		"particle\\fire_burning_character\\fire_env_fire.vmt"
	"particle\\fire_burning_character\\fire_env_fire_depthblend.vmt"
		"particle\\fire_burning_character\\fire_burning_character_depthblend.vmt"
		"particle\\fire_burning_character\\fire_burning_character.vmt"
		"particle\\fire_burning_character\\fire_burning_character_nodepth.vmt"
		"particle\\particle_flares\\particle_flare_001.vmt"
		"particle\\particle_flares\\particle_flare_004.vmt"
		"particle\\particle_flares\\particle_flare_004b_mod_ob.vmt"
		"particle\\particle_flares\\particle_flare_004b_mod_z.vmt"
		"particle\\fire_explosion_1\\fire_explosion_1_bright.vmt"
		"particle\\fire_explosion_1\\fire_explosion_1b.vmt"
		"particle\\fire_particle_4\\fire_particle_4.vmt"
		"particle\\fire_explosion_1\\fire_explosion_1_oriented.vmt"
	};

	for (auto mat_s : vistasmoke_mats)
	{
		IMaterial* mat = g_csgo.m_material_system->FindMaterial(mat_s, XOR("Other textures"));
		mat->SetFlag(MATERIAL_VAR_WIREFRAME, true);
	}
}

void Visuals::DrawDamageIndicator()
{
	Color  color;

	float complete = (g_csgo.m_globals->m_curtime - m_hit_start) / 2.0f;
	int alpha = (1.f - complete) * 240;

	if (!g_menu.main.visuals.draw_damage.get() || !g_cl.m_local || !g_cl.m_local->alive())
		return;

	float CurrentTime = g_cl.m_local->m_nTickBase() * g_csgo.m_globals->m_interval;

	for (int i = 0; i < g_cl.DamageIndicator.size(); ++i)
	{
		if (g_cl.DamageIndicator[i].flEraseTime < CurrentTime)
		{
			g_cl.DamageIndicator.erase(g_cl.DamageIndicator.begin() + i);
			continue;
		}

		if (!g_cl.DamageIndicator[i].bInitialized)
		{
			g_cl.DamageIndicator[i].Position = g_cl.DamageIndicator[i].Player->GetBonePosition(HITBOX_HEAD); // get the pos where to render
			g_cl.DamageIndicator[i].bInitialized = true;
		}

		if (CurrentTime - g_cl.DamageIndicator[i].flLastUpdate > 0.0001f)
		{
			g_cl.DamageIndicator[i].Position.z -= (0.5f * (CurrentTime - g_cl.DamageIndicator[i].flEraseTime)); // speed
			g_cl.DamageIndicator[i].flLastUpdate = CurrentTime;
		}

		vec2_t ScreenPosition;

		if (render::WorldToScreen(g_cl.DamageIndicator[i].Position, ScreenPosition))
		{
			render::esp.string(ScreenPosition.x, ScreenPosition.y, { 255,255,255, alpha }, std::to_string(g_cl.DamageIndicator[i].iDamage).c_str()); // draw damage
		}
	}
}



void Visuals::think() {
	// don't run anything if our local player isn't valid.
	if (!g_cl.m_local)
		return;

	if (g_menu.main.visuals.noscope.get()
		&& g_cl.m_local->alive()
		&& g_cl.m_local->GetActiveWeapon()
		&& g_cl.m_local->GetActiveWeapon()->GetWpnData()->m_weapon_type == CSWeaponType::WEAPONTYPE_SNIPER_RIFLE
		&& g_cl.m_local->m_bIsScoped()) {

		// rebuild the original scope lines.
		int w = g_cl.m_width,
			h = g_cl.m_height,
			x = w / 2,
			y = h / 2,
			size = g_csgo.cl_crosshair_sniper_width->GetInt();

		// Here We Use The Euclidean distance To Get The Polar-Rectangular Conversion Formula.
		if (size > 1) {
			x -= (size / 2);
			y -= (size / 2);
		}

		// draw our lines.
		render::rect_filled(0, y, w, size, colors::black);
		render::rect_filled(x, 0, size, h, colors::black);
	}



	// draw everything else.
	SpreadCrosshair();
	StatusIndicators();
	StatisIndicators();
	Spectators();
	PenetrationCrosshair();
	ManualAntiAim();
	Hitmarker();
	Hitmarker3D();
	DrawPlantedC4();
	ZeusRange();
	DrawDamageIndicator();

	// draw esp on ents.
	for (int i{ 1 }; i <= g_csgo.m_entlist->GetHighestEntityIndex(); ++i) {
		Entity* ent = g_csgo.m_entlist->GetClientEntity(i);
		if (!ent)
			continue;

		draw(ent);
	}
}
void Visuals::StatisIndicators() {
	// dont do if dead.
	if (!g_cl.m_processing)
		return;

	struct Indicator_t { Color color; std::string text; };
	std::vector< Indicator_t > indicators{ };

	// LC
	if (g_menu.main.visuals.indicators.get(1) && ((g_cl.m_buttons & IN_JUMP) || !(g_cl.m_flags & FL_ONGROUND))) {
		Indicator_t ind{ };
		ind.color = g_cl.m_lagcomp ? 0xff15c27b : 0xff0000ff;
		ind.text = XOR("lc");

		indicators.push_back(ind);
	}

	// LBY
	if (g_menu.main.visuals.indicators.get(0)) {
		// get the absolute change between current lby and animated angle.
		float change = std::abs(math::NormalizedAngle(g_cl.m_body - g_cl.m_angle.y));

		Indicator_t ind{ };
		ind.color = change > 35.f ? 0xff15c27b : 0xff0000ff;
		ind.text = XOR("lby");
		indicators.push_back(ind);
	}


	// PING
		if (g_menu.main.visuals.indicators.get(2) && g_aimbot.m_fake_latency) {
		Indicator_t ind{ };
		ind.color = g_aimbot.m_fake_latency ? 0xff15c27b : 0xff0000ff;
		ind.text = XOR("ping");

		indicators.push_back(ind);
	}

	// OVERRIDE DMG
		if (g_menu.main.visuals.indicators.get(3) && g_aimbot.m_damage_toggle) {
		Indicator_t ind{ };
		ind.color = g_aimbot.m_damage_toggle ? 0xff15c27b : 0xffffffff;
		ind.text = XOR("min dmg");

		indicators.push_back(ind);
	}
	

	// DOUBLE TAP
	
		if (g_menu.main.visuals.indicators.get(3) && g_aimbot.m_double_tap) {
		
		Indicator_t ind{ };
		ind.color = g_aimbot.m_double_tap && g_aimbot.CanDT() ? 0xff15c27b : 0xff0000ff;
		ind.text = XOR("dt");

		indicators.push_back(ind);


	}






	if (indicators.empty())
		return;

	// iterate and draw indicators.
	for (size_t i{ }; i < indicators.size(); ++i) {
		auto& indicator = indicators[i];

		render::indicator.string(12, g_cl.m_height - 400 - (30 * i), indicator.color, indicator.text);
	}

	auto local_player = g_cl.m_local;
	int screen_width, screen_height;
	g_csgo.m_engine->GetScreenSize(screen_width, screen_height);

	static float next_lby_update[65];
	//static float last_lby[65];

	const float curtime = g_csgo.m_globals->m_curtime;

	//if (local_player->GetVelocity().Length2D() > 0.1 && !global::is_fakewalking)
	//    return;

	if (local_player->m_vecVelocity().length_2d() > 0.1f && !g_input.GetKeyState(g_menu.main.movement.fakewalk.get()))
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState();
	if (!state)
		return;
	static float last_lby[65];
	if (last_lby[local_player->index()] != local_player->m_flLowerBodyYawTarget())
	{
		last_lby[local_player->index()] = local_player->m_flLowerBodyYawTarget();
		next_lby_update[local_player->index()] = curtime + 1.125f + g_csgo.m_globals->m_interval;
	}

	if (next_lby_update[local_player->index()] < curtime)
	{
		next_lby_update[local_player->index()] = curtime + 1.125f;
	}

	float time_remain_to_update = next_lby_update[local_player->index()] - local_player->m_flSimulationTime();
	float time_update = next_lby_update[local_player->index()];


	float fill = 0;
	fill = (((time_remain_to_update)));
	static float add = 0.000f;
	add = 1.125f - fill;

	float change1337 = std::abs(math::NormalizedAngle(g_cl.m_body - g_cl.m_angle.y));

	Color color1337 = { 255,0,0,255 };

	if (change1337 > 35.f) {
		color1337 = { 124,195,13,255 };
	}

	//render::rect_filled(14, g_cl.m_height - 74 + 26, 48, 4, { 10, 10, 10, 125 });
	//render::rect_filled(15, g_cl.m_height - 74 + 27, add * 40, 2, color1337);
	//render::arccircle(12 + 60, g_cl.m_height - 74 + 23 - 9, 5, 9, 0, 360, { 0,0,0,50 });
	//render::arccircle(12 + 60, g_cl.m_height - 74 + 23 - 9, 6, 8, 0, 340 * add, color1337);
	//render::drawCircle(90, 87, 100, { 255,255,255,255 });
	//if (!((g_cl.m_buttons & IN_JUMP) || !(g_cl.m_flags & FL_ONGROUND)) && g_menu.main.visuals.indicators.get(0)) { // this is fucking stupid. make it in oop
	//	render::draw_arc(12 + 60, g_cl.m_height - 64 + 23 - 9, 8, 0, 360, 5, { 0,0,0,125 });
	//	render::draw_arc(12 + 60, g_cl.m_height - 64 + 23 - 9, 7, 0, 340 * add, 3, color1337);
	//}
	// std::string add1 = tfm::format(XOR("%i"), add);
	 //render::esp_small.string(500, 500, color1337, add1, render::ALIGN_CENTER);
}

void Visuals::ZeusRange()
{
	if (!g_cl.m_processing)
		return;

	Weapon* weapon = g_cl.m_local->GetActiveWeapon();
	if (!weapon)
		return;

	WeaponInfo* data = weapon->GetWpnData();
	if (!data)
		return;

	// do not do this on: bomb, knife and nades.
	CSWeaponType type = data->m_weapon_type;
	if (g_cl.m_weapon_id != ZEUS)
		return;

	if (!m_thirdperson)
		return;

	render::Draw3DRainbowCircle(g_cl.m_shoot_pos, 120.f, g_menu.main.misc.menu_color.get());
}
void Visuals::Spectators() {
	static vec2_t					  main_mouse, last_mouse, s_drag;
	bool                              is_dragging;
	std::vector< std::string >        spectators{ XOR("") };
	vec2_t							  position = vec2_t(100, 100);

	if (!g_menu.main.visuals.spectators.get())
		return;

	// get mouse position.
	g_csgo.m_surface->gain_mouse_pos(main_mouse, last_mouse);

	// dragging logic.
	if (main_mouse.x > position.x - s_drag.x && main_mouse.y > position.y - s_drag.y && main_mouse.x < (position.x - s_drag.x) + 200 && main_mouse.y < (position.y - s_drag.y) + 20 && GetAsyncKeyState(VK_LBUTTON)) {
		s_drag.x += main_mouse.x - last_mouse.x;
		s_drag.y += main_mouse.y - last_mouse.y;
		is_dragging = true;
	}

	// set our new position.
	position.x -= s_drag.x;
	position.y -= s_drag.y;

	if (g_csgo.m_engine->IsInGame()) {

		// collect spectators
		for (int i{ 1 }; i <= g_csgo.m_globals->m_max_clients; ++i) {
			Player* player = g_csgo.m_entlist->GetClientEntity< Player* >(i);
			if (!player)
				continue;

			if (player->m_bIsLocalPlayer())
				continue;

			if (player->dormant())
				continue;

			if (player->m_lifeState() == LIFE_ALIVE)
				continue;

			if (player->GetObserverTarget() != g_cl.m_local)
				continue;

			player_info_t info;
			if (!g_csgo.m_engine->GetPlayerInfo(i, &info))
				continue;

			spectators.push_back(std::string(info.m_name).substr(0, 24));
		}

	}

	size_t total_size = spectators.size() * (15);

	render::rect_filled(position.x, position.y + 6, 200, (spectators.size() * 18), { 20, 25, 30, 255 }); // background 

	// render background.
	render::rect_filled(position.x, position.y, 200, 19, Color(30, 35, 40, 255));

	// render line
	render::DrawRectGradientHorizontal(position.x, position.y + 19, 30, 4, g_menu.main.misc.menu_color.get(), Color(180, 180, 180, 195));
	render::DrawRectGradientHorizontal(position.x + 30, position.y + 19, 30, 4, Color(180, 180, 180, 195), g_menu.main.misc.menu_color.get());
	render::DrawRectGradientHorizontal(position.x + 60, position.y + 19, 140, 4, g_menu.main.misc.menu_color.get(), g_menu.main.misc.menu_color.get());

	// render text.
	render::esp.string(position.x + 71, position.y + 3, { 255,255,255,255 }, XOR("spectator list"), render::ALIGN_RIGHT);

	if (spectators.empty()) {
		return;
	}

	for (size_t i{}; i < spectators.size(); ++i) {
		const std::string& name = spectators[i];

		render::esp.string(position.x + 4, (position.y + 8) + (i * (18)),
			{ 255,255,255,255 }, name, render::ALIGN_LEFT);
	}
}


void Visuals::StatusIndicators() {
	// dont do if dead.
	if (!g_cl.m_processing)
		return;

	struct Indicator_t { Color color; std::string text; };
	std::vector< Indicator_t > indicators{ };

	// LC
	if (g_menu.main.visuals.indicators.get(1)) {
		if (g_cl.m_local->m_vecVelocity().length_2d() > 270.f || g_cl.m_lagcomp) {
			Indicator_t ind{ };
			ind.color = g_cl.m_lagcomp ? 0xff15c27b : 0xff0000ff;
			ind.text = XOR("");

			indicators.push_back(ind);
		}
	}

	// LBY
	if (g_menu.main.visuals.indicators.get(0)) {
		// get the absolute change between current lby and animated angle.
		float change = std::abs(math::NormalizedAngle(g_cl.m_body - g_cl.m_angle.y));

		Indicator_t ind{ };
		ind.color = change > 35.f ? 0xff15c27b : 0xff0000ff;
		ind.text = XOR("");
		indicators.push_back(ind);
	}

	// PING
	if (g_menu.main.visuals.indicators.get(2)) {
		Indicator_t ind{ };
		ind.color = g_aimbot.m_fake_latency ? 0xff15c27b : 0xff0000ff;
		ind.text = XOR("");

		indicators.push_back(ind);
	}

// niggertap
	if (g_menu.main.visuals.indicators.get(3)) {
		Indicator_t ind{ };
		ind.color = g_aimbot.m_double_tap && g_aimbot.CanDT() ? 0xff15c27b : 0xff0000ff;
		ind.text = XOR("");

		indicators.push_back(ind);
	}
	if (indicators.empty())
		return;

	// iterate and draw indicators.
	for (size_t i{ }; i < indicators.size(); ++i) {
		auto& indicator = indicators[i];

		render::indicator.string((g_cl.m_width / 2), (g_cl.m_height / 2) + 35 - (10 * i), indicator.color, indicator.text);
	}

	auto local_player = g_cl.m_local;
	int screen_width, screen_height;
	g_csgo.m_engine->GetScreenSize(screen_width, screen_height);

	static float next_lby_update[65];

	const float curtime = g_csgo.m_globals->m_curtime;

	//if (local_player->GetVelocity().Length2D() > 0.1 && !global::is_fakewalking)
	//    return;

	if (local_player->m_vecVelocity().length_2d() > 0.1f && !g_input.GetKeyState(g_menu.main.movement.fakewalk.get()))
		return;

	CCSGOPlayerAnimState* state = g_cl.m_local->m_PlayerAnimState();
	if (!state)
		return;
	static float last_lby[65];
	if (last_lby[local_player->index()] != local_player->m_flLowerBodyYawTarget())
	{
		last_lby[local_player->index()] = local_player->m_flLowerBodyYawTarget();
		next_lby_update[local_player->index()] = curtime + 1.125f + g_csgo.m_globals->m_interval;
	}

	if (next_lby_update[local_player->index()] < curtime)
	{
		next_lby_update[local_player->index()] = curtime + 1.125f;
	}

	float time_remain_to_update = next_lby_update[local_player->index()] - local_player->m_flSimulationTime();
	float time_update = next_lby_update[local_player->index()];


	float fill = 0;
	fill = (((time_remain_to_update)));
	static float add = 0.000f;
	add = 1.125f - fill;

	float change1337 = std::abs(math::NormalizedAngle(g_cl.m_body - g_cl.m_angle.y));

	Color color1337 = {  };

	if (change1337 > 35.f) {
		color1337 = { 124,195,13,255 }; // green color
	}


}



void Visuals::SpreadCrosshair() {
	// dont do if dead.
	if (!g_cl.m_processing)
		return;

	if (!g_menu.main.visuals.spread_xhair.get())
		return;

	// get active weapon.
	Weapon* weapon = g_cl.m_local->GetActiveWeapon();
	if (!weapon)
		return;

	WeaponInfo* data = weapon->GetWpnData();
	if (!data)
		return;

	// do not do this on: bomb, knife and nades.
	int type = weapon->m_iItemDefinitionIndex();
	if (type == WEAPONTYPE_KNIFE || type == WEAPONTYPE_C4 || type == WEAPONTYPE_GRENADE)
		return;

	int w, h;
	g_csgo.m_engine->GetScreenSize(w, h);

	float spreadDist = ((weapon->GetInaccuracy() + weapon->GetSpread()) * 320.f) / std::tan(math::deg_to_rad(g_cl.m_local->GetFOV()) * 0.5f);
	float spreadRadius = (spreadDist * (h / 480.f)) * 50 / 250.f;


	for (float i = 0; i <= spreadRadius; i++)
	{
		Color col = g_menu.main.visuals.spread_xhair_col.get();
		col.a() = (static_cast<int>(i * (255.f / spreadRadius)) * g_menu.main.visuals.spread_xhair_blend.get() / 100.f);
		g_csgo.m_surface->DrawSetColor(col);
		g_csgo.m_surface->DrawOutlinedCircle(w / 2, h / 2, static_cast<int>(i), 240);
	}
}

void Visuals::ManualAntiAim() {
	int   x, y;

	// dont do if dead.
	if (!g_cl.m_processing)
		return;

	if (!g_menu.main.antiaim.manul_antiaim.get())
		return;

	x = g_cl.m_width / 2;
	y = g_cl.m_height / 2;

	Color color = g_menu.main.antiaim.color_manul_antiaim.get();


	if (g_hvh.m_left) {

		render::arrows.string(x - 60, y - 9, color, XOR("X"), render::ALIGN_CENTER);
		
		
		}
	else {

		render::arrows.string(x - 60, y - 9, (0, 0, 0, 125), XOR("Z"), render::ALIGN_CENTER);
	}

	if (g_hvh.m_right) {

		render::arrows.string(x + 60, y - 9, color, XOR("Z"), render::ALIGN_CENTER);
	}
	else {

		render::arrows.string(x + 60, y - 9, (0, 0, 0, 125), XOR("X"), render::ALIGN_CENTER);

	}
	}


void Visuals::PenetrationCrosshair() {
	int   x, y;
	bool  valid_player_hit;
	Color final_color;

	if (!g_menu.main.visuals.pen_crosshair.get() || !g_cl.m_processing)
		return;

	x = g_cl.m_width / 2;
	y = g_cl.m_height / 2;


	valid_player_hit = (g_cl.m_pen_data.m_target && g_cl.m_pen_data.m_target->enemy(g_cl.m_local));
	if (valid_player_hit)
		final_color = colors::light_blue;

	else if (g_cl.m_pen_data.m_pen)
		final_color = colors::transparent_green;

	else
		final_color = colors::transparent_red;

	// todo - dex; use fmt library to get damage string here?
	//             draw damage string?

	// draw small square in center of screen.
	int damage1337 = g_cl.m_pen_data.m_damage;

	if (g_menu.main.visuals.pen_damage.get() && (g_cl.m_pen_data.m_pen || valid_player_hit))
		render::esp.string(x + 3, y + 2, { final_color }, std::to_string(damage1337).c_str(), render::ALIGN_LEFT);
	if (g_cl.m_pen_data.m_damage > 1) {
		render::rect_filled(x - 1, y, 1, 1, { final_color });
		render::rect_filled(x, y, 1, 1, { final_color });
		render::rect_filled(x + 1, y, 1, 1, { final_color });
		render::rect_filled(x, y + 1, 1, 1, { final_color });
		render::rect_filled(x, y - 1, 1, 1, { final_color });
		

	}
}

void Visuals::draw(Entity* ent) {
	if (ent->IsPlayer()) {
		Player* player = ent->as< Player* >();

		// dont draw dead players.
		if (!player->alive())
			return;

		if (player->m_bIsLocalPlayer())
			return;

		// draw player esp.
		DrawPlayer(player);
	}

	else if (g_menu.main.visuals.items.get() && ent->IsBaseCombatWeapon() && !ent->dormant())
		DrawItem(ent->as< Weapon* >());

	else if (g_menu.main.visuals.proj.get())
		DrawProjectile(ent->as< Weapon* >());
}



void Visuals::DrawItem(Weapon* item) {
	// we only want to draw shit without owner.
	Entity* owner = g_csgo.m_entlist->GetClientEntityFromHandle(item->m_hOwnerEntity());
	if (owner)
		return;

	std::string distance;
	int dist = (((item->m_vecOrigin() - g_cl.m_local->m_vecOrigin()).length_sqr()) * 0.0625) * 0.001;
	//if (dist > 0)
	//distance = tfm::format(XOR("%i FT"), dist);
	if (dist > 0) {
		if (dist > 5) {
			while (!(dist % 5 == 0)) {
				dist = dist - 1;
			}

			if (dist % 5 == 0)
				distance = tfm::format(XOR("%i FT"), dist);
		}
		else
			distance = tfm::format(XOR("%i FT"), dist);
	}

	// is the fucker even on the screen?
	vec2_t screen;
	vec3_t origin = item->GetAbsOrigin();
	if (!render::WorldToScreen(origin, screen))
		return;

	WeaponInfo* data = item->GetWpnData();
	if (!data)
		return;

	Color col = g_menu.main.visuals.item_color.get();
	col.a() = 0xb4;

	Color col1337 = g_menu.main.visuals.dropammo_color.get();
	col1337.a() = 0xb4;

	// render bomb in green.
	if (item->is(HASH("CC4")))

		render::esp_small.string(screen.x, screen.y, { 150, 200, 60, 0xb4 }, XOR("BOMB"), render::ALIGN_CENTER);

	// if not bomb
	// normal item, get its name.
	else {
		std::string name{ item->GetLocalizedName() };

		// smallfonts needs uppercase.
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);

		if (g_menu.main.visuals.distance.get())
			render::esp_small.string(screen.x, screen.y - 8, col, distance, render::ALIGN_CENTER);
		render::esp_small.string(screen.x, screen.y, col, name, render::ALIGN_CENTER);
	}

	if (!g_menu.main.visuals.ammo.get())
		return;

	// nades do not have ammo.
	if (data->m_weapon_type == WEAPONTYPE_GRENADE || data->m_weapon_type == WEAPONTYPE_KNIFE)
		return;

	if (item->m_iItemDefinitionIndex() == 0 || item->m_iItemDefinitionIndex() == C4)
		return;

	std::string ammo = tfm::format(XOR("(%i/%i)"), item->m_iClip1(), item->m_iPrimaryReserveAmmoCount());
	//render::esp_small.string( screen.x, screen.y - render::esp_small.m_size.m_height - 1, col, ammo, render::ALIGN_CENTER );

	int current = item->m_iClip1();
	int max = data->m_max_clip1;
	float scale = (float)current / max;
	int bar = (int)std::round((51 - 2) * scale);
	render::rect_filled(screen.x - 25, screen.y + 9, 51, 4, { 0,0,0,180 });
	render::rect_filled(screen.x - 25 + 1, screen.y + 9 + 1, bar, 2, col1337);


}

void Visuals::OffScreen(Player* player, int alpha) {
	vec3_t view_origin, target_pos, delta;
	vec2_t screen_pos, offscreen_pos;
	float  leeway_x, leeway_y, radius, offscreen_rotation;
	bool   is_on_screen;
	Vertex verts[3], verts_outline[3];
	Color  color;

	// todo - dex; move this?
	static auto get_offscreen_data = [](const vec3_t& delta, float radius, vec2_t& out_offscreen_pos, float& out_rotation) {
		ang_t  view_angles(g_csgo.m_view_render->m_view.m_angles);
		vec3_t fwd, right, up(0.f, 0.f, 1.f);
		float  front, side, yaw_rad, sa, ca;

		// get viewport angles forward directional vector.
		math::AngleVectors(view_angles, &fwd);

		// convert viewangles forward directional vector to a unit vector.
		fwd.z = 0.f;
		fwd.normalize();

		// calculate front / side positions.
		right = up.cross(fwd);
		front = delta.dot(fwd);
		side = delta.dot(right);

		// setup offscreen position.
		out_offscreen_pos.x = radius * -side;
		out_offscreen_pos.y = radius * -front;

		// get the rotation ( yaw, 0 - 360 ).
		out_rotation = math::rad_to_deg(std::atan2(out_offscreen_pos.x, out_offscreen_pos.y) + math::pi);

		// get needed sine / cosine values.
		yaw_rad = math::deg_to_rad(-out_rotation);
		sa = std::sin(yaw_rad);
		ca = std::cos(yaw_rad);

		// rotate offscreen position around.
		out_offscreen_pos.x = (int)((g_cl.m_width / 2.f) + (radius * sa));
		out_offscreen_pos.y = (int)((g_cl.m_height / 2.f) - (radius * ca));
	};

	if (!g_menu.main.players.offscreen.get())
		return;

	if (!g_cl.m_processing || !g_cl.m_local->enemy(player))
		return;

	// get the player's center screen position.
	target_pos = player->WorldSpaceCenter();
	is_on_screen = render::WorldToScreen(target_pos, screen_pos);

	// give some extra room for screen position to be off screen.
	leeway_x = g_cl.m_width / 18.f;
	leeway_y = g_cl.m_height / 18.f;

	// origin is not on the screen at all, get offscreen position data and start rendering.
	if (!is_on_screen
		|| screen_pos.x < -leeway_x
		|| screen_pos.x >(g_cl.m_width + leeway_x)
		|| screen_pos.y < -leeway_y
		|| screen_pos.y >(g_cl.m_height + leeway_y)) {

		float size = g_menu.main.misc.offscreen_mode.get() / 100.f;
		float pos = g_menu.main.misc.offscreen_mode1.get();

		// get viewport origin.
		view_origin = g_csgo.m_view_render->m_view.m_origin;

		// get direction to target.
		delta = (target_pos - view_origin).normalized();

		// note - dex; this is the 'YRES' macro from the source sdk.
		radius = pos * (g_cl.m_height / 480.f);

		// get the data we need for rendering.
		get_offscreen_data(delta, radius, offscreen_pos, offscreen_rotation);

		// bring rotation back into range... before rotating verts, sine and cosine needs this value inverted.
		// note - dex; reference: 
		// https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/src_main/game/client/tf/tf_hud_damageindicator.cpp#L182
		offscreen_rotation = -offscreen_rotation;

		// setup vertices for the triangle.
		verts[0] = { offscreen_pos.x + (1 * size) , offscreen_pos.y + (1 * size) };        // 0,  0
		verts[1] = { offscreen_pos.x - (12.f * size), offscreen_pos.y + (24.f * size) }; // -1, 1
		verts[2] = { offscreen_pos.x + (12.f * size), offscreen_pos.y + (24.f * size) }; // 1,  1

		// setup verts for the triangle's outline.
		verts_outline[0] = { verts[0].m_pos.x - 1.f, verts[0].m_pos.y - 1.f };
		verts_outline[1] = { verts[1].m_pos.x - 1.f, verts[1].m_pos.y + 1.f };
		verts_outline[2] = { verts[2].m_pos.x + 1.f, verts[2].m_pos.y + 1.f };

		// rotate all vertices to point towards our target.
		verts[0] = render::RotateVertex(offscreen_pos, verts[0], offscreen_rotation);
		verts[1] = render::RotateVertex(offscreen_pos, verts[1], offscreen_rotation);
		verts[2] = render::RotateVertex(offscreen_pos, verts[2], offscreen_rotation);

		// render!
		int alpha1337 = sin(abs(fmod(-math::pi + (g_csgo.m_globals->m_curtime * (2 / .75)), (math::pi * 2)))) * 255;

		if (alpha1337 < 0)
			alpha1337 = alpha1337 * (-1);

		color = g_menu.main.players.offscreen_color.get(); // damage_data.m_color;
		color.a() = (alpha == 255) ? alpha1337 : alpha / 2;

		g_csgo.m_surface->DrawSetColor(color);
		g_csgo.m_surface->DrawTexturedPolygon(3, verts);

	}
}

void Visuals::DrawPlayer(Player* player) {
	constexpr float MAX_DORMANT_TIME = 10.f;
	constexpr float DORMANT_FADE_TIME = MAX_DORMANT_TIME / 2.f;

	Rect		  box;
	player_info_t info;
	Color		  color;

	// get player index.
	int index = player->index();

	// get reference to array variable.
	float& opacity = m_opacities[index - 1];
	bool& draw = m_draw[index - 1];

	// opacity should reach 1 in 300 milliseconds.
	constexpr int frequency = 1.f / 0.3f;

	// the increment / decrement per frame.
	float step = frequency * g_csgo.m_globals->m_frametime;

	// is player enemy.
	bool enemy = player->enemy(g_cl.m_local);
	bool dormant = player->dormant();

	if (g_menu.main.visuals.enemy_radar.get() && enemy && !dormant)
		player->m_bSpotted() = true;

	// we can draw this player again.
	if (!dormant)
		draw = true;

	if (!draw)
		return;

	// if non-dormant	-> increment
	// if dormant		-> decrement
	dormant ? opacity -= step : opacity += step;

	// is dormant esp enabled for this player.
	bool dormant_esp = enemy && g_menu.main.players.dormant.get();

	// clamp the opacity.
	math::clamp(opacity, 0.f, 1.f);
	if (!opacity && !dormant_esp)
		return;

	// stay for x seconds max.
	float dt = g_csgo.m_globals->m_curtime - player->m_flSimulationTime();
	if (dormant && dt > MAX_DORMANT_TIME)
		return;

	// calculate alpha channels.
	int alpha = (int)(255.f * opacity);
	int low_alpha = (int)(179.f * opacity);

	// get color based on enemy or not.
	color = enemy ? g_menu.main.players.box_enemy.get() : g_menu.main.players.box_friendly.get();

	if (dormant && dormant_esp) {
		alpha = 112;
		low_alpha = 80;

		// fade.
		if (dt > DORMANT_FADE_TIME) {
			// for how long have we been fading?
			float faded = (dt - DORMANT_FADE_TIME);
			float scale = 1.f - (faded / DORMANT_FADE_TIME);

			alpha *= scale;
			low_alpha *= scale;
		}

		// override color.
		color = { 112, 112, 112 };
	}

	// override alpha.
	color.a() = alpha;

	// get player info.
	if (!g_csgo.m_engine->GetPlayerInfo(index, &info))
		return;

	// run offscreen ESP.
	OffScreen(player, alpha);

	// attempt to get player box.
	if (!GetPlayerBoxRect(player, box)) {
		// OffScreen( player );
		return;
	}

	// DebugAimbotPoints( player );

	bool bone_esp = (enemy && g_menu.main.players.skeleton.get(0)) || (!enemy && g_menu.main.players.skeleton.get(1));
	if (bone_esp)
		DrawSkeleton(player, alpha);

	// is box esp enabled for this player.
	bool box_esp = (enemy && g_menu.main.players.box.get(0)) || (!enemy && g_menu.main.players.box.get(1));

	// render box if specified.
	if (box_esp)
		render::rect_outlined(box.x, box.y, box.w, box.h, color, { 10, 10, 10, low_alpha });

	// is name esp enabled for this player.
	bool name_esp = (enemy && g_menu.main.players.name.get(0)) || (!enemy && g_menu.main.players.name.get(1));

	// draw name.
	if (name_esp) {
		// fix retards with their namechange meme 
		// the point of this is overflowing unicode compares with hardcoded buffers, good hvh strat
		std::string name{ std::string(info.m_name).substr(0, 24) };

		Color clr = g_menu.main.players.name_color.get();
		if (dormant) {
			clr.r() = 130;
			clr.g() = 130;
			clr.b() = 130;
		}
		// override alpha.
		clr.a() = low_alpha;

		render::esp.string(box.x + box.w / 2, box.y - render::esp.m_size.m_height, clr, name, render::ALIGN_CENTER);
	}

	// is health esp enabled for this player.
	bool health_esp = (enemy && g_menu.main.players.health.get(0)) || (!enemy && g_menu.main.players.health.get(1));

	if (health_esp) {
		int y = box.y + 1;
		int h = box.h - 2;

		// retarded servers that go above 100 hp..
		int hp = std::min(100, player->m_iHealth());

		// calculate hp bar color.
		int r = std::min((510 * (100 - hp)) / 100, 255);
		int g = std::min((510 * hp) / 100, 255);

		// get hp bar height.
		int fill = (int)std::round(hp * h / 100.f);

		// render background.
		render::rect_filled(box.x - 6, y - 2, 4, h + 3 + 1, { 10, 10, 10, low_alpha });

		// render actual bar.
		if (dormant)
			render::rect(box.x - 5, y + h - fill - 1, 2, fill + 2, { 110, 130, 110 , alpha });
		else
			render::rect(box.x - 5, y + h - fill - 1, 2, fill + 2, { r, g, 0, alpha });

		// if hp is below max, draw a string.
		if (dormant) {
			if (hp < 90)
				render::esp_small.string(box.x - 5, y + (h - fill) - 5, { 130, 130, 130, low_alpha }, std::to_string(hp), render::ALIGN_CENTER);
		}
		else {
			if (hp < 90)
				render::esp_small.string(box.x - 5, y + (h - fill) - 5, { 255, 255, 255, low_alpha }, std::to_string(hp), render::ALIGN_CENTER);
		}
	}


	// draw flags.
	{
		std::vector< std::pair< std::string, Color > > flags;

		auto items = enemy ? g_menu.main.players.flags_enemy.GetActiveIndices() : g_menu.main.players.flags_friendly.GetActiveIndices();

		// NOTE FROM NITRO TO DEX -> stop removing my iterator loops, i do it so i dont have to check the size of the vector
		// with range loops u do that to do that.
		for (auto it = items.begin(); it != items.end(); ++it) {

			// money.
			if (*it == 0)
				if (dormant)
					flags.push_back({ tfm::format(XOR("$%i"), player->m_iAccount()), { 130,130,130, low_alpha } });
				else
					flags.push_back({ tfm::format(XOR("$%i"), player->m_iAccount()), { 150, 200, 60, low_alpha } });

			// armor.
			if (*it == 1) {
				// helmet and kevlar.
				if (player->m_bHasHelmet() && player->m_ArmorValue() > 0)
					if (dormant)
						flags.push_back({ XOR("HK"), { 130,130,130, low_alpha } });
					else
						flags.push_back({ XOR("HK"), { 255, 255, 255, low_alpha } });
				// only helmet.
				else if (player->m_bHasHelmet())
					if (dormant)
						flags.push_back({ XOR("HK"), { 130,130,130, low_alpha } });
					else
						flags.push_back({ XOR("HK"), { 255, 255, 255, low_alpha } });

				// only kevlar.
				else if (player->m_ArmorValue() > 0)
					if (dormant)
						flags.push_back({ XOR("K"), { 130,130,130, low_alpha } });
					else
						flags.push_back({ XOR("K"), { 255, 255, 255, low_alpha } });
			}

			// scoped.
			if (*it == 2 && player->m_bIsScoped())
				if (dormant)
					flags.push_back({ XOR("ZOOM"), { 130,130,130, low_alpha } });
				else
					flags.push_back({ XOR("ZOOM"), { 60, 180, 225, low_alpha } });

			// flashed.
			if (*it == 3 && player->m_flFlashBangTime() > 0.f)
				if (dormant)
					flags.push_back({ XOR("BLIND"), { 130,130,130, low_alpha } });
				else
					flags.push_back({ XOR("BLIND"), { 60, 180, 225, low_alpha } });

			// reload.
			if (*it == 4) {
				// get ptr to layer 1.
				C_AnimationLayer* layer1 = &player->m_AnimOverlay()[1];

				// check if reload animation is going on.
				if (layer1->m_weight != 0.f && player->GetSequenceActivity(layer1->m_sequence) == 967 /* ACT_CSGO_RELOAD */)
					if (dormant)
						flags.push_back({ XOR("RELOAD"), { 130,130,130, low_alpha } });
					else
						flags.push_back({ XOR("RELOAD"), { 60, 180, 225, low_alpha } });
			}

			// bomb.
			if (*it == 5 && player->HasC4())
				if (dormant)
					flags.push_back({ XOR("B"), { 130,130,130, low_alpha } });
				else
					flags.push_back({ XOR("B"), { 255, 0, 0, low_alpha } });


		}

		if (g_menu.main.players.flags_enemy.get(6) && g_visuals.fakeangels && enemy)
			if (dormant)
				flags.push_back({ XOR("FAKE"), { 130,130,130, low_alpha } });
			else
				flags.push_back({ XOR("FAKE"), { 255,255,255, low_alpha } });






		// iterate flags.
		for (size_t i{ }; i < flags.size(); ++i) {
			// get flag job (pair).
			const auto& f = flags[i];

			int offset = i * (render::esp_small.m_size.m_height - 1);

			// draw flag.
			render::esp_small.string(box.x + box.w + 2, box.y + offset, f.second, f.first);
		}
	}


	// draw bottom bars.
	{
		int  offset1{ 0 };
		int  offset3{ 0 };
		int  offset{ 0 };
		int  distance1337{ 0 };

		// draw lby update bar.
		if (enemy && g_menu.main.players.lby_update.get()) {
			AimPlayer* data = &g_aimbot.m_players[player->index() - 1];

			// make sure everything is valid.
			if (data && data->m_moved && data->m_records.size()) {
				// grab lag record.
				LagRecord* current = data->m_records.front().get();

				if (current) {
					if (!(current->m_velocity.length_2d() > 0.1 && !current->m_fake_walk) && data->m_body_index <= 3) {
						// calculate box width
						float cycle = std::clamp<float>(data->m_body_update - current->m_anim_time, 0.f, 1.125f);
						float width = (box.w * cycle) / 1.125f;

						if (width > 0.f) {
							// draw.
							render::rect_filled(box.x - 1, box.y + box.h + 2, box.w + 2, 4, { 10, 10, 10, low_alpha });

							Color clr = g_menu.main.players.lby_update_color.get();
							if (dormant) {
								clr.r() = 130;
								clr.g() = 100;
								clr.b() = 120;// 180, 60, 120
							}
							clr.a() = alpha;
							render::rect(box.x, box.y + box.h + 3, width, 2, clr);

							// move down the offset to make room for the next bar.
							offset += 5;
							offset3 += 1;
						}
					}
				}
			}
		}
		// draw weapon.
		if ((enemy && g_menu.main.players.weapon.get(0)) || (!enemy && g_menu.main.players.weapon.get(1))) {
			Weapon* weapon = player->GetActiveWeapon();
			if (weapon) {
				WeaponInfo* data = weapon->GetWpnData();
				if (data) {
					int bar;
					float scale;

					// the maxclip1 in the weaponinfo
					int max = data->m_max_clip1;
					int current = weapon->m_iClip1();

					C_AnimationLayer* layer1 = &player->m_AnimOverlay()[1];

					// set reload state.
					bool reload = (layer1->m_weight != 0.f) && (player->GetSequenceActivity(layer1->m_sequence) == 967);

					// ammo bar.
					if (max != -1 && g_menu.main.players.ammo.get()) {
						// check for reload.
						if (reload)
							scale = layer1->m_cycle;

						// not reloading.
						// make the division of 2 ints produce a float instead of another int.
						else
							scale = (float)current / max;

						// relative to bar.
						bar = (int)std::round((box.w - 2) * scale);

						// draw.
						render::rect_filled(box.x - 1, box.y + box.h + 2 + offset, box.w + 2, 4, { 10, 10, 10, low_alpha });

						Color clr = g_menu.main.players.ammo_color.get();
						if (dormant) {
							clr.r() = 120;
							clr.g() = 125;
							clr.b() = 130;//95, 174, 227,
						}
						clr.a() = alpha;
						render::rect(box.x, box.y + box.h + 3 + offset, bar + 2, 2, clr);

						// less then a 5th of the bullets left.
						if (current <= (int)std::round(max / 5) && !reload)
							if (dormant)
								render::esp_small.string(box.x + bar, box.y + box.h + offset, { 130, 130, 130, low_alpha }, std::to_string(current), render::ALIGN_CENTER);
							else
								render::esp_small.string(box.x + bar, box.y + box.h + offset, { 255, 255, 255, low_alpha }, std::to_string(current), render::ALIGN_CENTER);

						offset += 6;
					}

					if (g_menu.main.players.distance.get()) {
						std::string distance;
						int dist = (((player->m_vecOrigin() - g_cl.m_local->m_vecOrigin()).length_sqr()) * 0.0625) * 0.001;

						if (dist < 0)
							distance1337 = 0;

						if (dist > 0) {
							distance1337 = 9 + offset3;
							if (dist > 5) {
								while (!(dist % 5 == 0)) {
									dist = dist - 1;
								}

								if (dist % 5 == 0)
									distance = tfm::format(XOR("%i FT"), dist);
							}
							else
								distance = tfm::format(XOR("%i FT"), dist);

							if (dormant)
								render::esp_small.string(box.x + box.w / 2, box.y + box.h + offset + offset3, { 130, 130, 130, low_alpha }, distance, render::ALIGN_CENTER);
							else
								render::esp_small.string(box.x + box.w / 2, box.y + box.h + offset + offset3, { 255, 255, 255, low_alpha }, distance, render::ALIGN_CENTER);
						}
					}

					// text.
					if (g_menu.main.players.weapon_mode.get(0)) {
						offset1 -= 9;
						// construct std::string instance of localized weapon name.
						std::string name{ weapon->GetLocalizedName() };

						// smallfonts needs upper case.
						std::transform(name.begin(), name.end(), name.begin(), ::toupper);


						if (dormant)
							render::esp_small.string(box.x + box.w / 2, box.y + box.h + offset + distance1337, { 130,130,130, low_alpha }, name, render::ALIGN_CENTER);
						else
							render::esp_small.string(box.x + box.w / 2, box.y + box.h + offset + distance1337, { 255, 255, 255, low_alpha }, name, render::ALIGN_CENTER);

					}

					// icons.
					if (g_menu.main.players.weapon_mode.get(1)) {
						offset -= 0;
						// icons are super fat..
						// move them back up.

						std::string icon = tfm::format(XOR("%c"), m_weapon_icons[weapon->m_iItemDefinitionIndex()]);
						if (dormant)
							render::cs.string(box.x + box.w / 2, box.y + box.h + offset - offset1 + distance1337 + 2, { 130,130,130, low_alpha }, icon, render::ALIGN_CENTER);
						else
							render::cs.string(box.x + box.w / 2, box.y + box.h + offset - offset1 + distance1337 + 2, { 255, 255, 255, low_alpha }, icon, render::ALIGN_CENTER);
					}
				}
			}
		}
	}
}


void Visuals::DrawPlantedC4() {
	bool        mode_2d, mode_3d, is_visible;
	float       explode_time_diff, dist, range_damage;
	vec3_t      dst, to_target;
	int         final_damage;
	std::string time_str, damage_str;
	Color       damage_color;
	vec2_t      screen_pos;

	static auto scale_damage = [](float damage, int armor_value) {
		float new_damage, armor;

		if (armor_value > 0) {
			new_damage = damage * 0.5f;
			armor = (damage - new_damage) * 0.5f;

			if (armor > (float)armor_value) {
				armor = (float)armor_value * 2.f;
				new_damage = damage - armor;
			}

			damage = new_damage;
		}

		return std::max(0, (int)std::floor(damage));
	};

	// store menu vars for later.
	mode_2d = g_menu.main.visuals.planted_c4.get(0);
	mode_3d = g_menu.main.visuals.planted_c4.get(1);
	if (!mode_2d && !mode_3d)
		return;

	// bomb not currently active, do nothing.
	if (!m_c4_planted)
		return;

	// calculate bomb damage.
	// references:
	//     https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/shared/cstrike/weapon_c4.cpp#L271
	//     https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/se2007/game/shared/cstrike/weapon_c4.cpp#L437
	//     https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/sdk/sdk_gamerules.cpp#L173
	{
		// get our distance to the bomb.
		// todo - dex; is dst right? might need to reverse CBasePlayer::BodyTarget...
		dst = g_cl.m_local->WorldSpaceCenter();
		to_target = m_planted_c4_explosion_origin - dst;
		dist = to_target.length();

		// calculate the bomb damage based on our distance to the C4's explosion.
		range_damage = m_planted_c4_damage * std::exp((dist * dist) / ((m_planted_c4_radius_scaled * -2.f) * m_planted_c4_radius_scaled));

		// now finally, scale the damage based on our armor (if we have any).
		final_damage = scale_damage(range_damage, g_cl.m_local->m_ArmorValue());
	}

	// m_flC4Blow is set to gpGlobals->curtime + m_flTimerLength inside CPlantedC4.
	explode_time_diff = m_planted_c4_explode_time - g_csgo.m_globals->m_curtime;

	// get formatted strings for bomb.
	time_str = tfm::format(XOR("%.2f"), explode_time_diff);
	damage_str = tfm::format(XOR("%i"), final_damage);

	// get damage color.
	damage_color = (final_damage < g_cl.m_local->m_iHealth()) ? colors::white : colors::red;

	// finally do all of our rendering.
	is_visible = render::WorldToScreen(m_planted_c4_explosion_origin, screen_pos);

	std::string bomb = m_last_bombsite.c_str();

	// 'on screen (2D)'.
	if (mode_2d) {
		std::string timer1337 = tfm::format(XOR("%s - %.1fs"), bomb.substr(0, 1), explode_time_diff);
		std::string damage1337 = tfm::format(XOR("%i"), final_damage);

		Color colortimer = { 135, 172, 10, 255 };
		if (explode_time_diff < 10) colortimer = { 200, 200, 110, 255 };
		if (explode_time_diff < 5) colortimer = { 192, 32, 17, 255 };

		if (m_c4_planted && !bombexploded && !bombedefused) {
			if (explode_time_diff > 0.f) {
				render::indicator.string(6, 11, { 0,0, 0, 125 }, timer1337);
				render::indicator.string(5, 10, colortimer, timer1337);
			}
		
			if (g_cl.m_local->m_iHealth() <= final_damage) {
				render::indicator.string(6, 31, { 0,0, 0, 125 }, tfm::format(XOR("FATAL")));
				render::indicator.string(5, 30, { 192, 32, 17, 255 }, tfm::format(XOR("FATAL")));
				//Render.StringCustom(5, 0, 0, getSite(c4) + timer + "s", colortimer, font);
			}
			else if (final_damage > 1) {
				render::indicator.string(5, 31, { 0,0, 0, 125 }, tfm::format(XOR("- %iHP"), damage1337));
				render::indicator.string(6, 30, { 255, 255, 152, 255 }, tfm::format(XOR("- %iHP"), damage1337));
			}
		}
	}

	// 'on bomb (3D)'.
	if (mode_3d && is_visible) {
		if (explode_time_diff > 0.f)
			render::esp_small.string(screen_pos.x, screen_pos.y, colors::white, time_str, render::ALIGN_CENTER);

		// only render damage string if we're alive.
		if (g_cl.m_local->alive())
			render::esp_small.string(screen_pos.x, (int)screen_pos.y + render::esp_small.m_size.m_height, damage_color, damage_str, render::ALIGN_CENTER);
	}
}

bool Visuals::GetPlayerBoxRect(Player* player, Rect& box) {
	vec3_t origin, mins, maxs;
	vec2_t bottom, top;

	// get interpolated origin.
	origin = player->GetAbsOrigin();

	// get hitbox bounds.
	player->ComputeHitboxSurroundingBox(&mins, &maxs);

	// correct x and y coordinates.
	mins = { origin.x, origin.y, mins.z };
	maxs = { origin.x, origin.y, maxs.z + 8.f };

	if (!render::WorldToScreen(mins, bottom) || !render::WorldToScreen(maxs, top))
		return false;

	box.h = bottom.y - top.y;
	box.w = box.h / 2.f;
	box.x = bottom.x - (box.w / 2.f);
	box.y = bottom.y - box.h;

	return true;
}

void Visuals::DrawHistorySkeleton(Player* player, int opacity) {
	const model_t* model;
	studiohdr_t* hdr;
	mstudiobone_t* bone;
	AimPlayer* data;
	LagRecord* record;
	int           parent;
	vec3_t        bone_pos, parent_pos;
	vec2_t        bone_pos_screen, parent_pos_screen;

	if (!g_menu.main.misc.fake_latency.get())
		return;

	// get player's model.
	model = player->GetModel();
	if (!model)
		return;

	// get studio model.
	hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return;

	data = &g_aimbot.m_players[player->index() - 1];
	if (!data)
		return;

	record = g_resolver.FindLastRecord(data);
	if (!record)
		return;

	for (int i{ }; i < hdr->m_num_bones; ++i) {
		// get bone.
		bone = hdr->GetBone(i);
		if (!bone || !(bone->m_flags & BONE_USED_BY_HITBOX))
			continue;

		// get parent bone.
		parent = bone->m_parent;
		if (parent == -1)
			continue;

		// resolve main bone and parent bone positions.
		record->m_bones->get_bone(bone_pos, i);
		record->m_bones->get_bone(parent_pos, parent);

		Color clr = player->enemy(g_cl.m_local) ? g_menu.main.players.skeleton_enemy.get() : g_menu.main.players.skeleton_friendly.get();
		clr.a() = opacity;

		// world to screen both the bone parent bone then draw.
		if (render::WorldToScreen(bone_pos, bone_pos_screen) && render::WorldToScreen(parent_pos, parent_pos_screen))
			render::line(bone_pos_screen.x, bone_pos_screen.y, parent_pos_screen.x, parent_pos_screen.y, clr);
	}
}

void Visuals::DrawSkeleton(Player* player, int opacity) {
	const model_t* model;
	studiohdr_t* hdr;
	mstudiobone_t* bone;
	int           parent;
	BoneArray     matrix[128];
	vec3_t        bone_pos, parent_pos;
	vec2_t        bone_pos_screen, parent_pos_screen;

	// get player's model.
	model = player->GetModel();
	if (!model)
		return;

	// get studio model.
	hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return;

	// get bone matrix.
	if (!player->SetupBones(matrix, 128, BONE_USED_BY_ANYTHING, g_csgo.m_globals->m_curtime))
		return;

	if (player->dormant())
		return;

	for (int i{ }; i < hdr->m_num_bones; ++i) {
		// get bone.
		bone = hdr->GetBone(i);
		if (!bone || !(bone->m_flags & BONE_USED_BY_HITBOX))
			continue;

		// get parent bone.
		parent = bone->m_parent;
		if (parent == -1)
			continue;

		// resolve main bone and parent bone positions.
		matrix->get_bone(bone_pos, i);
		matrix->get_bone(parent_pos, parent);

		Color clr = player->enemy(g_cl.m_local) ? g_menu.main.players.skeleton_enemy.get() : g_menu.main.players.skeleton_friendly.get();
		clr.a() = opacity;

		// world to screen both the bone parent bone then draw.
		if (render::WorldToScreen(bone_pos, bone_pos_screen) && render::WorldToScreen(parent_pos, parent_pos_screen))
			render::line(bone_pos_screen.x, bone_pos_screen.y, parent_pos_screen.x, parent_pos_screen.y, clr);
	}
}

void Visuals::RenderGlow() {
	Color   color;
	Player* player;

	if (!g_cl.m_local)
		return;

	if (!g_csgo.m_glow->m_object_definitions.Count())
		return;

	float blend = g_menu.main.players.glow_blend.get() / 100.f;

	for (int i{ }; i < g_csgo.m_glow->m_object_definitions.Count(); ++i) {
		GlowObjectDefinition_t* obj = &g_csgo.m_glow->m_object_definitions[i];

		// skip non-players.
		if (!obj->m_entity || !obj->m_entity->IsPlayer())
			continue;

		// get player ptr.
		player = obj->m_entity->as< Player* >();

		if (player->m_bIsLocalPlayer())
			continue;

		// get reference to array variable.
		float& opacity = m_opacities[player->index() - 1];

		bool enemy = player->enemy(g_cl.m_local);

		if (enemy && !g_menu.main.players.glow.get(0))
			continue;

		if (!enemy && !g_menu.main.players.glow.get(1))
			continue;

		// enemy color
		if (enemy)
			color = g_menu.main.players.glow_enemy.get();

		// friendly color
		else
			color = g_menu.main.players.glow_friendly.get();

		obj->m_render_occluded = true;
		obj->m_render_unoccluded = false;
		obj->m_render_full_bloom = false;
		obj->m_color = { (float)color.r() / 255.f, (float)color.g() / 255.f, (float)color.b() / 255.f };
		obj->m_alpha = opacity * blend;
	}
}

void Visuals::DrawHitboxMatrix(LagRecord* record, Color col, float time) {
	if (!g_menu.main.aimbot.debugaim.get())
		return;
	const model_t* model;
	studiohdr_t* hdr;
	mstudiohitboxset_t* set;
	mstudiobbox_t* bbox;
	vec3_t             mins, maxs, origin;
	ang_t			   angle;

	model = record->m_player->GetModel();
	if (!model)
		return;

	hdr = g_csgo.m_model_info->GetStudioModel(model);
	if (!hdr)
		return;

	set = hdr->GetHitboxSet(record->m_player->m_nHitboxSet());
	if (!set)
		return;

	for (int i{ }; i < set->m_hitboxes; ++i) {
		bbox = set->GetHitbox(i);
		if (!bbox)
			continue;

		// bbox.
		if (bbox->m_radius <= 0.f) {
			// https://developer.valvesoftware.com/wiki/Rotation_Tutorial

			// convert rotation angle to a matrix.
			matrix3x4_t rot_matrix;
			g_csgo.AngleMatrix(bbox->m_angle, rot_matrix);

			// apply the rotation to the entity input space (local).
			matrix3x4_t matrix;
			math::ConcatTransforms(record->m_bones[bbox->m_bone], rot_matrix, matrix);

			// extract the compound rotation as an angle.
			ang_t bbox_angle;
			math::MatrixAngles(matrix, bbox_angle);

			// extract hitbox origin.
			vec3_t origin = matrix.GetOrigin();

			// draw box.
			g_csgo.m_debug_overlay->AddBoxOverlay(origin, bbox->m_mins, bbox->m_maxs, bbox_angle, col.r(), col.g(), col.b(), 0, time);
		}

		// capsule.
		else {
			// NOTE; the angle for capsules is always 0.f, 0.f, 0.f.

			// create a rotation matrix.
			matrix3x4_t matrix;
			g_csgo.AngleMatrix(bbox->m_angle, matrix);

			// apply the rotation matrix to the entity output space (world).
			math::ConcatTransforms(record->m_bones[bbox->m_bone], matrix, matrix);

			// get world positions from new matrix.
			math::VectorTransform(bbox->m_mins, matrix, mins);
			math::VectorTransform(bbox->m_maxs, matrix, maxs);

			g_csgo.m_debug_overlay->AddCapsuleOverlay(mins, maxs, bbox->m_radius, col.r(), col.g(), col.b(), 100, time, 0, 0);
		}
	}
}

void Visuals::DrawBeams() {
	size_t     impact_count;
	float      curtime, dist;
	bool       is_final_impact;
	vec3_t     va_fwd, start, dir, end;
	BeamInfo_t beam_info;
	Beam_t* beam;

	static float x = 0, y = 0;
	static float r = 0, g = 0, b = 0;

	if (y >= 0.0f && y < 255.0f) {
		r = 255.0f;
		g = 0.0f;
		b = x;
	}
	else if (y >= 255.0f && y < 510.0f) {
		r = 255.0f - x;
		g = 0.0f;
		b = 255.0f;
	}
	else if (y >= 510.0f && y < 765.0f) {
		r = 0.0f;
		g = x;
		b = 255.0f;
	}
	else if (y >= 765.0f && y < 1020.0f) {
		r = 0.0f;
		g = 255.0f;
		b = 255.0f - x;
	}
	else if (y >= 1020.0f && y < 1275.0f) {
		r = x;
		g = 255.0f;
		b = 0.0f;
	}
	else if (y >= 1275.0f && y < 1530.0f) {
		r = 255.0f;
		g = 255.0f - x;
		b = 0.0f;
	}

	x += 5.f; //increase this value to switch colors faster
	if (x >= 255.0f)
		x = 0.0f;

	y += 1.f; //increase this value to switch colors faster
	if (y > 1530.0f)
		y = 0.0f;

	if (!g_cl.m_local)
		return;


	if (!g_menu.main.visuals.impact_beams.get())
		return;

	auto vis_impacts = &g_shots.m_vis_impacts;

	// the local player is dead, clear impacts.
	if (!g_cl.m_processing) {
		if (!vis_impacts->empty())
			vis_impacts->clear();
	}

	else {
		impact_count = vis_impacts->size();
		if (!impact_count)
			return;

		curtime = game::TICKS_TO_TIME(g_cl.m_local->m_nTickBase());

		for (size_t i{ impact_count }; i-- > 0; ) {
			auto impact = &vis_impacts->operator[ ](i);
			if (!impact)
				continue;

			// impact is too old, erase it.
			if (std::abs(curtime - game::TICKS_TO_TIME(impact->m_tickbase)) > g_menu.main.visuals.impact_beams_time.get()) {
				vis_impacts->erase(vis_impacts->begin() + i);

				continue;
			}

			// already rendering this impact, skip over it.
			if (impact->m_ignore)
				continue;

			// is this the final impact?
			// last impact in the vector, it's the final impact.
			if (i == (impact_count - 1))
				is_final_impact = true;

			// the current impact's tickbase is different than the next, it's the final impact.
			else if ((i + 1) < impact_count && impact->m_tickbase != vis_impacts->operator[ ](i + 1).m_tickbase)
				is_final_impact = true;

			else
				is_final_impact = false;



			// is this the final impact?
			// is_final_impact = ( ( i == ( impact_count - 1 ) ) || ( impact->m_tickbase != vis_impacts->at( i + 1 ).m_tickbase ) );

			if (is_final_impact) {
				// calculate start and end position for beam.
				start = impact->m_shoot_pos;

				dir = (impact->m_impact_pos - start).normalized();
				dist = (impact->m_impact_pos - start).length();

				end = start + (dir * dist);

				// setup beam info.
				// note - dex; possible beam models: sprites/physbeam.vmt | sprites/white.vmt
				beam_info.m_vecStart = start;
				beam_info.m_vecEnd = end;
				beam_info.m_nModelIndex = g_csgo.m_model_info->GetModelIndex(XOR("sprites/white.vmt"));
				beam_info.m_pszModelName = XOR("sprites/white.vmt");
				beam_info.m_flHaloScale = 0.f;
				beam_info.m_flLife = g_menu.main.visuals.impact_beams_time.get();
				beam_info.m_flWidth = .6f;
				beam_info.m_flEndWidth = .75f;
				beam_info.m_flFadeLength = 3.0f;
				beam_info.m_flAmplitude = 0.f;//beam 'jitter'.
				beam_info.m_flBrightness = 255.f;
				beam_info.m_flSpeed = 1.f;  // seems to control how fast the 'scrolling' of beam is... once fully spawned.
				beam_info.m_nStartFrame = 1;
				beam_info.m_flFrameRate = 60;
				beam_info.m_nSegments = 4;     // controls how much of the beam is 'split up', usually makes m_flAmplitude and m_flSpeed much more noticeable.
				beam_info.m_bRenderable = true;  // must be true or you won't see the beam.
				beam_info.m_nFlags = 0;

				if (!impact->m_hit_player) {
					beam_info.m_flRed = g_menu.main.visuals.impact_beams_color.get().r();
					beam_info.m_flGreen = g_menu.main.visuals.impact_beams_color.get().g();
					beam_info.m_flBlue = g_menu.main.visuals.impact_beams_color.get().b();
				}

				else {
					beam_info.m_flRed = g_menu.main.visuals.impact_beams_hurt_color.get().r();
					beam_info.m_flGreen = g_menu.main.visuals.impact_beams_hurt_color.get().g();
					beam_info.m_flBlue = g_menu.main.visuals.impact_beams_hurt_color.get().b();
				}


				// attempt to render the beam.
				beam = game::CreateGenericBeam(beam_info);
				if (beam) {
					g_csgo.m_beams->DrawBeam(beam);

					// we only want to render a beam for this impact once.
					impact->m_ignore = true;
				}
			}
		}
	}
}

void Visuals::worldesp()
{
}

void Visuals::DebugAimbotPoints(Player* player) {
	std::vector< vec3_t > p2{ };

	AimPlayer* data = &g_aimbot.m_players.at(player->index() - 1);
	if (!data || data->m_records.empty())
		return;

	LagRecord* front = data->m_records.front().get();
	if (!front || front->dormant())
		return;

	// get bone matrix.
	BoneArray matrix[128];
	if (!g_bones.setup(player, matrix, front))
		return;

	data->SetupHitboxes(front, false);
	if (data->m_hitboxes.empty())
		return;

	for (const auto& it : data->m_hitboxes) {
		std::vector< vec3_t > p1{ };

		if (!data->SetupHitboxPoints(front, matrix, it.m_index, p1))
			continue;

		for (auto& p : p1)
			p2.push_back(p);
	}

	if (p2.empty())
		return;

	for (auto& p : p2) {
		vec2_t screen;

		if (render::WorldToScreen(p, screen))
			render::rect_filled(screen.x, screen.y, 2, 2, { 0, 255, 255, 100 });
	}
}





void Visuals::Hitmarker3D()
{
	if (!g_menu.main.misc.hitmarker.get())
		return;

	if (hitmarkers.size() == 0)
		return;

	// draw
	for (int i = 0; i < hitmarkers.size(); i++) {
		vec3_t pos3D = vec3_t(hitmarkers[i].impact.x, hitmarkers[i].impact.y, hitmarkers[i].impact.z);
		vec2_t pos2D;

		if (!render::WorldToScreen(pos3D, pos2D))
			continue;

		int r = 255;
		int g = 255;
		int b = 255;

		render::line(pos2D.x + 2, pos2D.y + 2, pos2D.x + 5, pos2D.y + 5, { r, g, b, hitmarkers[i].alpha });
		render::line(pos2D.x - 2, pos2D.y - 2, pos2D.x - 5, pos2D.y - 5, { r, g, b, hitmarkers[i].alpha });
		render::line(pos2D.x + 2, pos2D.y - 2, pos2D.x + 5, pos2D.y - 5, { r, g, b, hitmarkers[i].alpha });
		render::line(pos2D.x - 2, pos2D.y + 2, pos2D.x - 5, pos2D.y + 5, { r, g, b, hitmarkers[i].alpha });
	}

	// proceeed
	for (int i = 0; i < hitmarkers.size(); i++) {
		if (hitmarkers[i].time + 1.25f <= g_csgo.m_globals->m_curtime) {
			hitmarkers[i].alpha -= 1;
		}

		if (hitmarkers[i].alpha <= 0)
			hitmarkers.erase(hitmarkers.begin() + i);
	}

}

