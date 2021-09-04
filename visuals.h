#pragma once

struct OffScreenDamageData_t {
    float m_time, m_color_step;
    Color m_color;

    __forceinline OffScreenDamageData_t() : m_time{ 0.f }, m_color{ colors::white } {}
    __forceinline OffScreenDamageData_t(float time, float m_color_step, Color color) : m_time{ time }, m_color{ color } {}
};

struct impact_info {
    float x, y, z;
    float time;
};

struct hitmarker_info {
    impact_info impact;
    int alpha;
    float time;
};

struct WorldHitmarkerData_t {
    float m_time, m_alpha;
    float m_pos_x, m_pos_y, m_pos_z;

    vec2_t m_world_pos;
    bool m_world_to_screen;
};

class Visuals {
public:
    std::array< bool, 64 >                  m_draw;
    std::array< float, 2048 >               m_opacities;
    std::array< OffScreenDamageData_t, 64 > m_offscreen_damage;
    vec2_t                                  m_crosshair;
    bool                                    m_thirdperson;
    int                                    fakeangels[64];
    float					                m_hit_start, m_hit_end, m_hit_duration;

    std::vector<impact_info>                impacts;
    std::vector<hitmarker_info>             hitmarkers;

    // info about planted c4.
    bool                                    planted;
    bool                                    bombexploded;
    bool                                    bombedefused;
    std::string                             m_bombsite;
    bool                                    bombeindefuse;
    float                                   m_planted_c4_defuse_time;
    bool        m_c4_planted;
    Entity* m_planted_c4;
    float       m_planted_c4_explode_time;
    vec3_t      m_planted_c4_explosion_origin;
    float       m_planted_c4_damage;
    float       m_planted_c4_radius;
    float       m_planted_c4_radius_scaled;
    float       miss;
    float       miss1;
    float       miss2;
    std::string m_last_bombsite;

    IMaterial* smoke1;
    IMaterial* smoke2;
    IMaterial* smoke3;
    IMaterial* smoke4;

    std::unordered_map< int, char > m_weapon_icons = {
        { DEAGLE, 'A' },
        { ELITE, 'B' },
        { FIVESEVEN, 'C' },
        { GLOCK, 'D' },
        { AK47, 'W' },
        { AUG, 'U' },
        { AWP, 'Z' },
        { FAMAS, 'R' },
        { G3SG1, 'X' },
        { GALIL, 'Q' },
        { M249, 'G' },
        { M4A4, 'S' },
        { MAC10, 'K' },
        { P90, 'M' },
        { UMP45, 'L' },
        { XM1014, 'B' },
        { BIZON, 'M' },
        { MAG7, 'd' },
        { NEGEV, 'f' },
        { SAWEDOFF, 'c' },
        { TEC9, 'H' },
        { ZEUS, 'h' },
        { P2000, 'E' },
        { MP7, 'N' },
        { MP9, '0' },
        { NOVA, 'e' },
        { P250, 'F' },
        { SCAR20, 'Y' },
        { SG553, 'V' },
        { SSG08, 'a' },
        { KNIFE_CT, ']' },
        { FLASHBANG, 'i' },
        { HEGRENADE, 'j' },
        { SMOKE, 'k' },
        { MOLOTOV, 'l' },
        { DECOY, 'm' },
        { FIREBOMB, 'n' },
        { C4, 'o' },
        { KNIFE_T, '[' },
        { M4A1S, 'T' },
        { USPS, 'G' },
        { CZ75A, 'I' },
        { REVOLVER, 'J' },
        { KNIFE_BAYONET, '1' },
        { KNIFE_FLIP, '2' },
        { KNIFE_GUT, '3' },
        { KNIFE_KARAMBIT, '4' },
        { KNIFE_M9_BAYONET, '5' },
        { KNIFE_HUNTSMAN, '6' },
        { KNIFE_FALCHION, '7' },
        { KNIFE_BOWIE, '7' },
        { KNIFE_BUTTERFLY, '8' },
        { KNIFE_SHADOW_DAGGERS, '9' },
    };

public:
    static void ModulateWorld();
    void ThirdpersonThink();
    void IndicateAngles();
    void Hitmarker();
    void Hitmarker3D();
    void NoMolly();
    void DrawDamageIndicator();
    void NoSmoke();
    
    void think();
    void ZeusRange();
    void Spectators();
    void FakeFlick();
    void StatusIndicators();
    void SpreadCrosshair();
    void StatisIndicators();
    void ManualAntiAim();
    void PenetrationCrosshair();
    void DrawPlantedC4();
    void draw(Entity* ent);
    void DrawProjectile(Weapon* ent);
    void DrawItem(Weapon* item);
    void OffScreen(Player* player, int alpha);
    void DrawPlayer(Player* player);
    bool GetPlayerBoxRect(Player* player, Rect& box);
    void DrawHistorySkeleton(Player* player, int opacity);
    void DrawSkeleton(Player* player, int opacity);
    void RenderGlow();
    void DrawHitboxMatrix(LagRecord* record, Color col, float time);
    void DrawBeams();
    void worldesp();
    void DebugAimbotPoints(Player* player);
};

extern Visuals g_visuals;