#define _CRT_SECURE_NO_DEPRECATE
#include <windows.h>
#include <windowsx.h>
#include <Richedit.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <algorithm>
#include <vector>
#include <fstream>
#include <map>
#include <string>
#include <sstream>
#include "../Common/common.h"
#include "../Common/GetCommonDirectory.h"
#include "resource.h"

std::string base_dir = "edxSilkroadLoader5";

//-----------------------------------------------------------------------------
// Opcode isimleri (Silkroad Online paket komutları)
//-----------------------------------------------------------------------------

std::map<WORD, std::string> opcodeNames;

void InitOpcodeNames()
{
	// === GLOBAL ===
	opcodeNames[0x2001] = "GLOBAL_IDENTIFICATION";
	opcodeNames[0x2002] = "GLOBAL_PING";
	opcodeNames[0x5000] = "GLOBAL_HANDSHAKE";
	opcodeNames[0x6000] = "GLOBAL_IDENTIFICATION_ERROR";
	opcodeNames[0x6003] = "GLOBAL_CERTIFICATION_REQUEST";
	opcodeNames[0x6008] = "GLOBAL_FORWARD_REQUEST";
	opcodeNames[0x600D] = "GLOBAL_MASSIVE";
	opcodeNames[0x9000] = "GLOBAL_HANDSHAKE_ACCEPT";
	opcodeNames[0xA003] = "GLOBAL_CERTIFICATION_RESPONSE";
	opcodeNames[0xA008] = "GLOBAL_FORWARD_RESPONSE";
	
	// === GATEWAY/LOGIN ===
	opcodeNames[0x0FF1] = "GATEWAY_SERVER_READY";
	opcodeNames[0x0FF2] = "GATEWAY_INIT_COMPLETE";
	opcodeNames[0x0FF3] = "GATEWAY_CAPTCHA_ACCEPT";
	opcodeNames[0x1002] = "GATEWAY_CAPTCHA_CHALLENGE";
	opcodeNames[0x1003] = "GATEWAY_CAPTCHA_RESPONSE";
	opcodeNames[0x2322] = "GATEWAY_LOGIN_IBUV_CHALLENGE";
	opcodeNames[0x6100] = "GATEWAY_PATCH_REQUEST";
	opcodeNames[0x6101] = "GATEWAY_SERVERLIST_REQUEST";
	opcodeNames[0x6102] = "GATEWAY_LOGIN_REQUEST";
	opcodeNames[0x6104] = "GATEWAY_NOTICE_REQUEST";
	opcodeNames[0x6106] = "GATEWAY_SHARD_LIST_PING_REQUEST";
	opcodeNames[0x6323] = "GATEWAY_LOGIN_IBUV_ANSWER";
	opcodeNames[0xA100] = "GATEWAY_PATCH_RESPONSE";
	opcodeNames[0xA101] = "GATEWAY_SERVERLIST_RESPONSE";
	opcodeNames[0xA102] = "GATEWAY_LOGIN_RESPONSE";
	opcodeNames[0xA104] = "GATEWAY_NOTICE_RESPONSE";
	opcodeNames[0xA106] = "GATEWAY_SHARD_LIST_PING_RESPONSE";
	opcodeNames[0xA323] = "GATEWAY_LOGIN_IBUV_RESULT";
	
	// === CHARACTER SELECTION ===
	opcodeNames[0x7001] = "CHARACTER_SELECTION_JOIN_REQUEST";
	opcodeNames[0x7007] = "CHARACTER_SELECTION_REQUEST";
	opcodeNames[0x7450] = "CHARACTER_SELECTION_RENAME_REQUEST";
	opcodeNames[0xB001] = "CHARACTER_SELECTION_JOIN_RESPONSE";
	opcodeNames[0xB007] = "CHARACTER_SELECTION_RESPONSE";
	opcodeNames[0xB450] = "CHARACTER_SELECTION_RENAME_RESPONSE";
	
	// === CHARACTER ===
	opcodeNames[0x3011] = "CHARACTER_DEATH";
	opcodeNames[0x3012] = "CHARACTER_TELEPORT_COMPLETE";
	opcodeNames[0x3013] = "CHARACTER_DATA";
	opcodeNames[0x303D] = "CHARACTER_STATS";
	opcodeNames[0x3053] = "RESURRECT_AT_TOWN";
	opcodeNames[0x3091] = "CHARACTER_EMOTICON_REQUEST";
	opcodeNames[0x34A5] = "CHARACTER_DATA_BEGIN";
	opcodeNames[0x34A6] = "CHARACTER_DATA_END";
	opcodeNames[0x34B5] = "CHARACTER_RESET";
	opcodeNames[0x34B6] = "CHARACTER_TELEPORT_ACCEPT";
	opcodeNames[0x7021] = "CHARACTER_MOVEMENT_REQUEST";
	opcodeNames[0x7023] = "CHARACTER_FORWARD_REQUEST";
	opcodeNames[0x7024] = "CHARACTER_TURN_REQUEST";
	opcodeNames[0x704F] = "CHARACTER_CHANGE_MOVESTATE_REQUEST";
	opcodeNames[0x7050] = "CHARACTER_INCREASE_STR_REQUEST";
	opcodeNames[0x7051] = "CHARACTER_INCREASE_INT_REQUEST";
	opcodeNames[0x7059] = "CHARACTER_CHANGE_RESURRECT_POINT_REQUEST";
	opcodeNames[0x7158] = "CHARACTER_CLIENTINFO_UPDATE";
	opcodeNames[0x7402] = "CHARACTER_UPDATE_AUTOINVEST";
	opcodeNames[0x7516] = "CHARACTER_CHANGE_PVPCAPE_REQUEST";
	opcodeNames[0x70A7] = "CHARACTER_CHANGE_BODYSTATE_REQUEST";
	opcodeNames[0xB050] = "CHARACTER_INCREASE_STR_RESPONSE";
	opcodeNames[0xB051] = "CHARACTER_INCREASE_INT_RESPONSE";
	opcodeNames[0xB059] = "CHARACTER_CHANGE_RESURRECT_POINT_RESPONSE";
	opcodeNames[0xB0A7] = "CHARACTER_CHANGE_BODYSTATE_RESPONSE";
	opcodeNames[0xB516] = "CHARACTER_CHANGE_PVPCAPE_RESPONSE";
	
	// === ENTITY ===
	opcodeNames[0x3015] = "ENTITY_SPAWN";
	opcodeNames[0x3016] = "ENTITY_DESPAWN";
	opcodeNames[0x3017] = "ENTITY_GROUPSPAWN_BEGIN";
	opcodeNames[0x3018] = "ENTITY_GROUPSPAWN_END";
	opcodeNames[0x3019] = "ENTITY_GROUPSPAWN_DATA";
	opcodeNames[0x3036] = "ENTITY_ANIMATION_PICKUP";
	opcodeNames[0x3041] = "ENTITY_COOLDOWN_START";
	opcodeNames[0x3042] = "ENTITY_COOLDOWN_CANCEL";
	opcodeNames[0x304D] = "ENTITY_REMOVE_OWNERSHIP";
	opcodeNames[0x304E] = "ENTITY_UPDATE_POINTS";
	opcodeNames[0x3056] = "ENTITY_UPDATE_EXPERIANCE";
	opcodeNames[0x3057] = "ENTITY_UPDATE_STATUS";
	opcodeNames[0x30BF] = "ENTITY_UPDATE_STATUS";
	opcodeNames[0x30D0] = "ENTITY_UPDATE_SPEED";
	opcodeNames[0x30EF] = "ENTITY_UPDATE_HOSTILITY";
	opcodeNames[0x30FF] = "ENTITY_UPDATE_GUILD";
	opcodeNames[0x3091] = "ENTITY_EMOTION";
	opcodeNames[0x3206] = "ENTITY_SCROLL_EFFECT";
	opcodeNames[0x3207] = "ENTITY_MASK";
	opcodeNames[0x3256] = "ENTITY_UPDATE_GUILD_NICKNAME";
	opcodeNames[0xB021] = "ENTITY_UPDATE_MOVEMENT";
	opcodeNames[0xB023] = "ENTITY_UPDATE_POSITION";
	opcodeNames[0xB024] = "ENTITY_UPDATE_ANGLE";
	opcodeNames[0xB402] = "ENTITY_UPDATE_AUTOINVEST";
	
	// === CHAT ===
	opcodeNames[0x3026] = "CHAT_RECEIVE";
	opcodeNames[0x7025] = "CHAT_REQUEST";
	opcodeNames[0xB025] = "CHAT_RESPONSE";
	
	// === ACTION/COMBAT ===
	opcodeNames[0x7045] = "ACTION_SELECT_REQUEST";
	opcodeNames[0x7046] = "ACTION_TALK_REQUEST";
	opcodeNames[0x704B] = "ACTION_DESELECT_REQUEST";
	opcodeNames[0x705B] = "ACTION_CANCEL_REQUEST";
	opcodeNames[0x7074] = "ACTION_COMMAND_REQUEST";
	opcodeNames[0xB045] = "ACTION_SELECT_RESPONSE";
	opcodeNames[0xB046] = "ACTION_TALK_RESPONSE";
	opcodeNames[0xB04B] = "ACTION_DESELECT_RESPONSE";
	opcodeNames[0xB070] = "ACTION_RESPONSE_1";
	opcodeNames[0xB071] = "ACTION_RESPONSE_2";
	opcodeNames[0xB072] = "ACTION_RESPONSE_3";
	opcodeNames[0xB074] = "ACTION_COMMAND_RESPONSE";
	opcodeNames[0xB0BD] = "ACTION_RESPONSE_4";
	
	// === INVENTORY ===
	opcodeNames[0x3038] = "INVENTORY_SLOT_UPDATE";
	opcodeNames[0x3040] = "INVENTORY_ITEM_UPDATE";
	opcodeNames[0x3052] = "ITEM_DURABILITY_UPDATE";
	opcodeNames[0x305C] = "INVENTORY_ITEM_CONSUMED";
	opcodeNames[0x3092] = "INVENTORY_SIZE_UPDATE";
	opcodeNames[0x3201] = "INVENTORY_AMMO_UPDATE";
	opcodeNames[0x7034] = "INVENTORY_ACTION_REQUEST";
	opcodeNames[0x703E] = "REPAIR_REQUEST";
	opcodeNames[0x704C] = "INVENTORY_ITEM_USE_REQUEST";
	opcodeNames[0xB034] = "INVENTORY_ACTION_RESPONSE";
	opcodeNames[0xB03E] = "REPAIR_RESPONSE";
	opcodeNames[0xB04C] = "INVENTORY_ITEM_USE_RESPONSE";

	// === STORAGE ===
	opcodeNames[0x3047] = "STORAGE_OPEN_RESPONSE";
	opcodeNames[0x3049] = "STORAGE_SIZE"; // 6 * 5 * 5 = 150 pages
	opcodeNames[0x703C] = "STORAGE_OPEN_REQUEST";
	
	// === SKILL ===
	opcodeNames[0x70A1] = "SKILL_LEARN_REQUEST";
	opcodeNames[0x70A2] = "SKILL_MASTERY_LEARN_REQUEST";
	opcodeNames[0x7202] = "SKILL_WITHDRAW_REQUEST";
	opcodeNames[0x7203] = "SKILL_MASTERY_WITHDRAW_REQUEST";
	opcodeNames[0xB0A1] = "SKILL_LEARN_RESPONSE";
	opcodeNames[0xB0A2] = "SKILL_MASTERY_LEARN_RESPONSE";
	opcodeNames[0xB202] = "SKILL_WITHDRAW_RESPONSE";
	opcodeNames[0xB203] = "SKILL_MASTERY_WITHDRAW_RESPONSE";
	
	// === PARTY ===
	opcodeNames[0x3065] = "PARTY_CREATE_FROM_MATCHING";
	opcodeNames[0x3068] = "PARTY_DISTRIBUTION";
	opcodeNames[0x306E] = "PARTY_MATCHING_JOIN_RESPONSE";
	opcodeNames[0x3864] = "PARTY_UPDATE";
	opcodeNames[0x3865] = "PARTY_CREATE";
	opcodeNames[0x7060] = "PARTY_CREATE_OR_INVITE_REQUEST";
	opcodeNames[0x7061] = "PARTY_LEAVE";
	opcodeNames[0x7063] = "PARTY_BANISH";
	opcodeNames[0x7069] = "PARTY_MATCHING_FORM_REQUEST";
	opcodeNames[0x706A] = "PARTY_MATCHING_CHANGE_REQUEST";
	opcodeNames[0x706B] = "PARTY_MATCHING_DELETE_REQUEST";
	opcodeNames[0x706C] = "PARTY_MATCHING_LIST_REQUEST";
	opcodeNames[0x706D] = "PARTY_MATCHING_JOIN_REQUEST";
	opcodeNames[0x7519] = "PARTY_DIMENSION_SUMMON_REQUEST";
	opcodeNames[0xB060] = "PARTY_CREATE_RESPONSE";
	opcodeNames[0xB062] = "PARTY_INVITE_RESPONSE";
	opcodeNames[0xB069] = "PARTY_MATCHING_FORM_RESPONSE";
	opcodeNames[0xB06A] = "PARTY_MATCHING_CHANGE_RESPONSE";
	opcodeNames[0xB06B] = "PARTY_MATCHING_DELETE_RESPONSE";
	opcodeNames[0xB06C] = "PARTY_MATCHING_LIST_RESPONSE";
	opcodeNames[0xB06D] = "PARTY_MATCHING_JOIN_RESPONSE";
	opcodeNames[0xB519] = "PARTY_DIMENSION_SUMMON_RESPONSE";
	
	// === GUILD ===
	opcodeNames[0x3100] = "GUILD_REMOVE";
	opcodeNames[0x3101] = "GUILD_DATA";
	opcodeNames[0x3102] = "UNION_DATA";
	opcodeNames[0x3109] = "GUILD_WAR_DATA";
	opcodeNames[0x3253] = "GUILD_STORAGE_GOLD";
	opcodeNames[0x3255] = "GUILD_STORAGE_LIST";
	opcodeNames[0x3257] = "ENTITY_UPDATE_CREST";
	opcodeNames[0x38F5] = "GUILD_UPDATE";
	opcodeNames[0x3908] = "GUILD_ELECTION_UPDATE";
	opcodeNames[0x7030] = "RECALL_REQUEST";
	opcodeNames[0x7031] = "RECALL_RESPONSE";
	opcodeNames[0x70F0] = "GUILD_CREATE_REQUEST";
	opcodeNames[0x70F1] = "GUILD_DISBAND_REQUEST";
	opcodeNames[0x70F3] = "GUILD_INVITE_REQUEST";
	opcodeNames[0x70F4] = "GUILD_WITHDRAW_REQUEST";
	opcodeNames[0x70F9] = "GUILD_UPDATE_NOTICE_REQUEST";
	opcodeNames[0x70FA] = "GUILD_PROMOTE_REQUEST";
	opcodeNames[0x70FB] = "GUILD_UNION_INVITE_REQUEST";
	opcodeNames[0x70FC] = "GUILD_UNION_SECEDE_REQUEST";
	opcodeNames[0x70FD] = "GUILD_UNION_EXPEL_REQUEST";
	opcodeNames[0x70FF] = "GUILD_UPDATE_SIEGEAUTH_REQUEST";
	opcodeNames[0x7103] = "GUILD_TRANSFER_REQUEST";
	opcodeNames[0x7104] = "GUILD_UPDATE_PERMISSIONS_REQUEST";
	opcodeNames[0x7105] = "GUILD_ELECTION_REQUEST";
	opcodeNames[0x7106] = "GUILD_ELECTION_PARTICIPATE_REQUEST";
	opcodeNames[0x7107] = "GUILD_ELECTION_VOTE";
	opcodeNames[0x7110] = "GUILD_WAR_START_REQUEST";
	opcodeNames[0x7112] = "GUILD_WAR_END_REQUEST";
	opcodeNames[0x7114] = "GUILD_WAR_REWARD_REQUEST";
	opcodeNames[0x7250] = "GUILD_STORAGE_OPEN_REQUEST";
	opcodeNames[0x7251] = "GUILD_STORAGE_CLOSE_REQUEST";
	opcodeNames[0x7252] = "GUILD_STORAGE_LIST_REQUEST";
	opcodeNames[0x7256] = "GUILD_UPDATE_NICKNAME_REQUEST";
	opcodeNames[0x7258] = "GUILD_GP_DONATE_REQUEST";
	opcodeNames[0x7501] = "GUILD_GP_HISTORY_REQUEST";
	opcodeNames[0xB031] = "RECALL_RESPONSE";
	opcodeNames[0xB0F0] = "GUILD_CREATE_RESPONSE";
	opcodeNames[0xB0F1] = "GUILD_DISBAND_RESPONSE";
	opcodeNames[0xB0F3] = "GUILD_INVITE_RESPONSE";
	opcodeNames[0xB0F4] = "GUILD_WITHDRAW_RESPONSE";
	opcodeNames[0xB0F9] = "GUILD_UPDATE_NOTICE_RESPONSE";
	opcodeNames[0xB0FA] = "GUILD_PROMOTE_RESPONSE";
	opcodeNames[0xB0FB] = "GUILD_UNION_INVITE_RESPONSE";
	opcodeNames[0xB0FC] = "GUILD_UNION_SECEDE_RESPONSE";
	opcodeNames[0xB0FD] = "GUILD_UNION_EXPEL_RESPONSE";
	opcodeNames[0xB0FF] = "GUILD_UPDATE_SIEGEAUTH_RESPONSE";
	opcodeNames[0xB103] = "GUILD_TRANSFER_RESPONSE";
	opcodeNames[0xB104] = "GUILD_UPDATE_PERMISSIONS_RESPONSE";
	opcodeNames[0xB105] = "GUILD_ELECTION_RESPONSE";
	opcodeNames[0xB106] = "GUILD_ELECTION_PARTICIPATE_RESPONSE";
	opcodeNames[0xB110] = "GUILD_WAR_START_RESPONSE";
	opcodeNames[0xB114] = "GUILD_WAR_REWARD_RESPONSE";
	opcodeNames[0xB250] = "GUILD_STORAGE_OPEN_RESPONSE";
	opcodeNames[0xB251] = "GUILD_STORAGE_CLOSE_RESPONSE";
	opcodeNames[0xB256] = "GUILD_UPDATE_NICKNAME_RESPONSE";
	opcodeNames[0xB258] = "GUILD_GP_DONATE_RESPONSE";
	opcodeNames[0xB501] = "GUILD_GP_HISTORY_RESPONSE";
	
	// === ALCHEMY ===
	opcodeNames[0x7150] = "ALCHEMY_STRENGTHENING_REQUEST";
	opcodeNames[0x7151] = "ALCHEMY_ATTRIBUTE_REQUEST";
	opcodeNames[0x7155] = "ALCHEMY_DISJOIN_REQUEST";
	opcodeNames[0x7157] = "ALCHEMY_DISMANTLE_REQUEST";
	opcodeNames[0xB150] = "ALCHEMY_STRENGTHENING_RESPONSE";
	opcodeNames[0xB151] = "ALCHEMY_ATTRIBUTE_RESPONSE";
	opcodeNames[0xB155] = "ALCHEMY_DISJOIN_RESPONSE";
	opcodeNames[0xB157] = "ALCHEMY_DISMANTLE_RESPONSE";
	
	// === EXCHANGE ===
	opcodeNames[0x3085] = "EXCHANGE_STARTED";
	opcodeNames[0x3086] = "EXCHANGE_CONFIRMED";
	opcodeNames[0x3087] = "EXCHANGE_APPROVED";
	opcodeNames[0x3088] = "EXCHANGE_CANCELED";
	opcodeNames[0x3089] = "EXCHANGE_UPDATE";
	opcodeNames[0x308C] = "EXCHANGE_UPDATE_ITEMS";
	opcodeNames[0x7081] = "EXCHANGE_START_REQUEST";
	opcodeNames[0x7082] = "EXCHANGE_CONFIRM_REQUEST";
	opcodeNames[0x7083] = "EXCHANGE_APPROVE_REQUEST";
	opcodeNames[0x7084] = "EXCHANGE_CANCEL_REQUEST";
	opcodeNames[0xB081] = "EXCHANGE_START_RESPONSE";
	opcodeNames[0xB082] = "EXCHANGE_CONFIRM_RESPONSE";
	opcodeNames[0xB083] = "EXCHANGE_APPROVE_RESPONSE";
	opcodeNames[0xB084] = "EXCHANGE_CANCEL_RESPONSE";
	
	// === STALL ===
	opcodeNames[0x30B7] = "ENTITY_STALL_ACTION";
	opcodeNames[0x30B8] = "ENTITY_STALL_CREATE";
	opcodeNames[0x30B9] = "ENTITY_STALL_DESTROY";
	opcodeNames[0x30BB] = "ENTITY_UPDATE_STALL_NAME";
	opcodeNames[0x70B1] = "STALL_CREATE_REQUEST";
	opcodeNames[0x70B2] = "STALL_DESTROY_REQUEST";
	opcodeNames[0x70B3] = "STALL_TALK_REQUEST";
	opcodeNames[0x70B4] = "STALL_BUY_REQUEST";
	opcodeNames[0x70B5] = "STALL_LEAVE_REQUEST";
	opcodeNames[0x70BA] = "STALL_UPDATE_REQUEST";
	opcodeNames[0xB0B1] = "STALL_CREATE_RESPONSE";
	opcodeNames[0xB0B2] = "STALL_DESTROY_RESPONSE";
	opcodeNames[0xB0B3] = "STALL_TALK_RESPONSE";
	opcodeNames[0xB0B4] = "STALL_BUY_RESPONSE";
	opcodeNames[0xB0B5] = "STALL_LEAVE_RESPONSE";
	opcodeNames[0xB0BA] = "STALL_UPDATE_RESPONSE";
	
	// === QUEST ===
	opcodeNames[0x30D4] = "QUEST_UPDATE_1";
	opcodeNames[0x30D5] = "QUEST_UPDATE_2";
	opcodeNames[0x30D6] = "QUEST_ACCEPT_UPDATE";
	opcodeNames[0x30D7] = "QUEST_SERVER_TIME";
	opcodeNames[0x30E7] = "QUEST_UPDATE_3";
	opcodeNames[0x30EC] = "QUEST_UPDATE_4";
	opcodeNames[0x3514] = "QUEST_UPDATE_5";
	opcodeNames[0x70D9] = "QUEST_ABANDON_REQUEST";
	opcodeNames[0x7515] = "QUEST_REQUEST";
	opcodeNames[0xB0D9] = "QUEST_ABANDON_RESPONSE";
	opcodeNames[0xB515] = "QUEST_RESPONSE";
	
	// === COMMUNITY (FRIEND/MEMO) ===
	opcodeNames[0x3303] = "COMMUNITY_FRIEND_ADD_RESPONSE";
	opcodeNames[0x3305] = "COMMUNITY_FRIEND_DATA";
	opcodeNames[0x3B07] = "COMMUNITY_UPDATE";
	opcodeNames[0x7302] = "COMMUNITY_FRIEND_ADD_REQUEST";
	opcodeNames[0x7304] = "COMMUNITY_FRIEND_DELETE_REQUEST";
	opcodeNames[0x7308] = "COMMUNITY_MEMO_OPEN_REQUEST";
	opcodeNames[0x7309] = "COMMUNITY_MEMO_SEND_REQUEST";
	opcodeNames[0x730A] = "COMMUNITY_MEMO_DELETE_REQUEST";
	opcodeNames[0x730B] = "COMMUNITY_MEMO_LIST_REQUEST";
	opcodeNames[0x730C] = "COMMUNITY_MEMO_GROUP_SEND_REQUEST";
	opcodeNames[0x730D] = "COMMUNITY_BLOCK_UPDATE_REQUEST";
	opcodeNames[0xB302] = "COMMUNITY_FRIEND_ADD_RESPONSE";
	opcodeNames[0xB304] = "COMMUNITY_FRIEND_DELETE_RESPONSE";
	opcodeNames[0xB308] = "COMMUNITY_MEMO_OPEN_RESPONSE";
	opcodeNames[0xB309] = "COMMUNITY_MEMO_SEND_RESPONSE";
	opcodeNames[0xB30A] = "COMMUNITY_MEMO_DELETE_RESPONSE";
	opcodeNames[0xB30B] = "COMMUNITY_MEMO_LIST_RESPONSE";
	opcodeNames[0xB30C] = "COMMUNITY_MEMO_GROUP_SEND_RESPONSE";
	opcodeNames[0xB30D] = "COMMUNITY_BLOCK_UPDATE_RESPONSE";
	
	// === FORTRESS ===
	opcodeNames[0x34D2] = "FORTRESS_BATTLE_ARENA_UPDATE";
	opcodeNames[0x385F] = "FORTRESS_UPDATE";
	opcodeNames[0x705D] = "FORTRESS_RETURN_REQUEST";
	opcodeNames[0x705E] = "FORTRESS_ACTION_REQUEST";
	opcodeNames[0xB05D] = "FORTRESS_RETURN_RESPONSE";
	opcodeNames[0xB05E] = "FORTRESS_ACTION_RESPONSE";
	
	// === COS (COMPANION/PET) ===
	opcodeNames[0x30C8] = "COS_DATA";
	opcodeNames[0x30C9] = "COS_UPDATE";
	opcodeNames[0x30CA] = "COS_STATE_UPDATE";
	opcodeNames[0x70C5] = "COS_COMMAND";
	opcodeNames[0x70C6] = "COS_TERMINATION_REQUEST";
	opcodeNames[0x7116] = "COS_UNSUMMON_REQUEST";
	opcodeNames[0x70CB] = "COS_UPDATE_MOUNTSTATE_REQUEST";
	opcodeNames[0x7117] = "COS_NAMING_OR_UNSUMMON_REQUEST";
	opcodeNames[0x7420] = "COS_UPDATE_SETTINGS_REQUEST";
	opcodeNames[0xB0C6] = "COS_TERMINATION_RESPONSE";
	opcodeNames[0xB0CB] = "COS_UPDATE_MOUNTSTATE_RESPONSE";
	opcodeNames[0xB420] = "COS_UPDATE_SETTINGS_RESPONSE";
	
	// === PK (PLAYER KILL) ===
	opcodeNames[0x30CD] = "PK_UPDATE_PENALTY";
	opcodeNames[0x30CE] = "PK_UPDATE_DAILY";
	opcodeNames[0x30D2] = "DEATH_PENALTY_INFO";
	opcodeNames[0x30D3] = "PK_UPDATE_LEVEL";
	
	// === CONSIGNMENT ===
	opcodeNames[0x350D] = "CONSIGNMENT_UPDATE";
	opcodeNames[0x3530] = "CONSIGNMENT_BUFF_ADD";
	opcodeNames[0x3531] = "CONSIGNMENT_BUFF_UPDATE";
	opcodeNames[0x3532] = "CONSIGNMENT_BUFF_REMOVE";
	opcodeNames[0x7506] = "CONSIGNMENT_DETAIL_REQUEST";
	opcodeNames[0x7507] = "CONSIGNMENT_CLOSE_REQUEST";
	opcodeNames[0x7508] = "CONSIGNMENT_REGISTER_REQUEST";
	opcodeNames[0x7509] = "CONSIGNMENT_UNREGISTER_REQUEST";
	opcodeNames[0x750A] = "CONSIGNMENT_BUY_REQUEST";
	opcodeNames[0x750B] = "CONSIGNMENT_SETTLE_REQUEST";
	opcodeNames[0x750C] = "CONSIGNMENT_SEARCH_REQUEST";
	opcodeNames[0x750E] = "CONSIGNMENT_LIST_REQUEST";
	opcodeNames[0xB506] = "CONSIGNMENT_DETAIL_RESPONSE";
	opcodeNames[0xB507] = "CONSIGNMENT_CLOSE_RESPONSE";
	opcodeNames[0xB508] = "CONSIGNMENT_REGISTER_RESPONSE";
	opcodeNames[0xB509] = "CONSIGNMENT_UNREGISTER_RESPONSE";
	opcodeNames[0xB50A] = "CONSIGNMENT_BUY_RESPONSE";
	opcodeNames[0xB50B] = "CONSIGNMENT_SETTLE_RESPONSE";
	opcodeNames[0xB50C] = "CONSIGNMENT_SEARCH_RESPONSE";
	opcodeNames[0xB50E] = "CONSIGNMENT_LIST_RESPONSE";
	
	// === DOWNLOAD ===
	opcodeNames[0x1001] = "DOWNLOAD_FILE_DATA";
	opcodeNames[0x6004] = "DOWNLOAD_FILE_REQUEST";
	opcodeNames[0xA004] = "DOWNLOAD_FILE_COMPLETE";
	
	// === ENVIRONMENT/WEATHER ===
	opcodeNames[0x3020] = "CHARACTER_CELESTIAL_POSITION";
	opcodeNames[0x3027] = "ENVIRONMENT_CELESTIAL_UPDATE";
	opcodeNames[0x3809] = "ENVIRONMENT_WEATHER_UPDATE";
	
	// === ACADEMY ===
	opcodeNames[0x347F] = "ACADEMY_MATCHING_JOIN_RESPONSE";
	opcodeNames[0x3C80] = "ACADEMY_DATA_1";
	opcodeNames[0x3C81] = "ACADEMY_DATA";
	opcodeNames[0x3C82] = "ACADEMY_DATA_2";
	opcodeNames[0x7470] = "ACADEMY_CREATE_REQUEST";
	opcodeNames[0x7471] = "ACADEMY_DISBAND_REQUEST";
	opcodeNames[0x7473] = "ACADEMY_KICK_REQUEST";
	opcodeNames[0x7474] = "ACADEMY_LEAVE_REQUEST";
	opcodeNames[0x7475] = "ACADEMY_GRADE_REQUEST";
	opcodeNames[0x7477] = "ACADEMY_UPDATE_COMMENT_REQUEST";
	opcodeNames[0x7478] = "HONOR_RANK_LIST_REQUEST";
	opcodeNames[0x747A] = "ACADEMY_MATCHING_REGISTRATION_REQUEST";
	opcodeNames[0x747B] = "ACADEMY_MATCHING_CHANGE_REQUEST";
	opcodeNames[0x747C] = "ACADEMY_MATCHING_DELETE_REQUEST";
	opcodeNames[0x747D] = "ACADEMY_MATCHING_LIST_REQUEST";
	opcodeNames[0x747E] = "ACADEMY_MATCHING_JOIN_REQUEST";
	opcodeNames[0xB470] = "ACADEMY_CREATE_RESPONSE";
	opcodeNames[0xB471] = "ACADEMY_DISBAND_RESPONSE";
	opcodeNames[0xB473] = "ACADEMY_KICK_RESPONSE";
	opcodeNames[0xB474] = "ACADEMY_LEAVE_RESPONSE";
	opcodeNames[0xB477] = "ACADEMY_UPDATE_COMMENT_RESPONSE";
	opcodeNames[0xB478] = "HONOR_RANK_LIST_RESPONSE";
	opcodeNames[0xB47A] = "ACADEMY_MATCHING_REGISTRATION_RESPONSE";
	opcodeNames[0xB47B] = "ACADEMY_MATCHING_CHANGE_RESPONSE";
	opcodeNames[0xB47C] = "ACADEMY_MATCHING_DELETE_RESPONSE";
	opcodeNames[0xB47D] = "ACADEMY_MATCHING_LIST_RESPONSE";
	opcodeNames[0xB47E] = "ACADEMY_MATCHING_JOIN_RESPONSE";

	// === JOB ===
	opcodeNames[0x70E1] = "JOB_LEAGUE_JOIN_REQUEST";
	opcodeNames[0x70E4] = "JOB_RANKING_REQUEST";
	opcodeNames[0x70E6] = "JOB_PREVIOUS_INFO_REQUEST";
	opcodeNames[0xB0E1] = "JOB_LEAGUE_JOIN_RESPONSE";
	opcodeNames[0xB0E4] = "JOB_RANKING_RESPONSE";
	opcodeNames[0xB0E6] = "JOB_PREVIOUS_INFO_RESPONSE";
	
	// === AUTH ===
	opcodeNames[0x6103] = "AUTH_REQUEST";
	opcodeNames[0xA103] = "AUTH_RESPONSE";

	// === BUFF ===
	opcodeNames[0x3159] = "BUFF_ITEM_CHAIN";
	opcodeNames[0x315B] = "BUFF_ITEM_ACTIVE";
	opcodeNames[0x325F] = "BUFF_TITLE_ACTIVATE";
	opcodeNames[0x3261] = "BUFF_TITLE_STATUS";
	
	// === SILK ===
	opcodeNames[0x3153] = "SILK_UPDATE";
	opcodeNames[0x7118] = "MAGIC_POP_REQUEST";
	opcodeNames[0x7119] = "MAGIC_POP_CLAIM_REWARD_REQUEST";
	opcodeNames[0x711A] = "SILK_HISTORY_REQUEST";
	opcodeNames[0xB118] = "MAGIC_POP_RESPONSE";
	opcodeNames[0xB119] = "MAGIC_POP_CLAIM_REWARD_RESPONSE";
	opcodeNames[0xB11A] = "SILK_HISTORY_RESPONSE";
	
	// === LOGOUT ===
	opcodeNames[0x300A] = "LOGOUT_SUCCESS";
	opcodeNames[0x7005] = "LOGOUT_REQUEST";
	opcodeNames[0x7006] = "LOGOUT_CANCEL_REQUEST";
	opcodeNames[0xB005] = "LOGOUT_RESPONSE";
	opcodeNames[0xB006] = "LOGOUT_CANCEL_RESPONSE";
	
	// === TAP ===
	opcodeNames[0x34E1] = "TAP_ICON";
	opcodeNames[0x74DF] = "TAP_INFO_REQUEST";
	opcodeNames[0x74E0] = "TAP_UPDATE_REQUEST";
	opcodeNames[0xB4DF] = "TAP_INFO_RESPONSE";
	opcodeNames[0xB4E0] = "TAP_UPDATE_RESPONSE";

	// === TRADE ===
	opcodeNames[0x74D4] = "TRADE_EXPORT_PRICES_REQUEST";
	opcodeNames[0xB4D4] = "TRADE_EXPORT_PRICES_RESPONSE";
	
	// === GUIDE ===
	opcodeNames[0x70EA] = "GUIDE_REQUEST";
	opcodeNames[0xB0EA] = "GUIDE_RESPONSE";
	
	// === OPERATOR ===
	opcodeNames[0x7010] = "OPERATOR_REQUEST";
	opcodeNames[0xB010] = "OPERATOR_RESPONSE";

	// === TELEPORT ===
	opcodeNames[0x705A] = "TELEPORT_EXECUTE_REQUEST";
	opcodeNames[0xB05A] = "TELEPORT_EXECUTE_RESPONSE";
	
	// === OTHER ===
	opcodeNames[0x300C] = "NOTIFY";
	opcodeNames[0x3080] = "INVITE_REQUEST_OR_RESPONSE";
}

const char* GetOpcodeName(WORD opcode)
{
	std::map<WORD, std::string>::iterator it = opcodeNames.find(opcode);
	if(it != opcodeNames.end())
		return it->second.c_str();
	return "UNKNOWN";
}

//-----------------------------------------------------------------------------

template <typename t>
t HexStringToInteger(const std::string & str)
{
	t val;
	std::stringstream ss;
	ss << std::hex << str;
	ss >> val;
	return val;
}

//-----------------------------------------------------------------------------

// Main GUI thread
DWORD WINAPI GuiThread(LPVOID lpParam);

// Window procedure for the GUI
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Handle to the gui thread
HANDLE hGuiThread = 0;

bool bDoLogging = true;

// Rich edit controls
HMODULE hRichLib = 0;
HWND hEdit = 0;
HANDLE hFont = 0;

// Log file
FILE * logFile = 0;
int bDoLog = true;

// Paket filtreleme seçenekleri
bool bShowC2S = true;                       // Client->Server paketleri göster
bool bShowS2C = true;                       // Server->Client paketleri göster
std::vector<DWORD> showOnlyOpcodes;         // Sadece bu opcode'ları göster (beyaz liste)
std::vector<DWORD> filterOpcodes;           // Bu opcode'ları yoksay/gizle (kara liste)

HWND hGui = 0;

// Save and load settings
void SaveFilter();
void LoadFilter();

// Ignore opcodes
bool AddIgnoreOpcode(DWORD opcode);
void RemoveIgnoreOpcode(DWORD opcode);
void ClearIgnoreOpcodes();

// Show only opcodes
bool AddAllowOpcode(DWORD opcode);
void RemoveAllowOpcode(DWORD opcode);
void ClearAllowOpcodes();

// Destination
void ShowCtoS(bool show);
void ShowStoC(bool show);
void ToggleCtoS();
void ToggleStoC();

// Get destination
BOOL GetStoC();
BOOL GetCtoS();

// Get lists of opcodes
std::vector<DWORD> GetAllowedOpcodes();
std::vector<DWORD> GetIgnoredOpcodes();

// Appends text to the rich edit view
void AppendText(bool doShow, char *cText);

// Mevcut paketin bilgileri
WORD packet_opcode = 0;      // Şu anda işlenen paketin opcode'u (komut numarası)
bool packet_show = false;     // Bu paketin gösterilip gösterilmeyeceği

//
extern HMODULE globalInstance;

//-------------------------------------------------------------------------

void Analyzer_Setup()
{
	// Opcode isimlerini başlat
	InitOpcodeNames();
	
	hGuiThread = CreateThread(0, 0, GuiThread, 0, 0, 0);
}

//-------------------------------------------------------------------------

void Analyzer_Cleanup()
{
	if(hGuiThread) TerminateThread(hGuiThread, 0);
	if(hFont) DeleteObject(hFont);
	if(hRichLib) FreeLibrary(hRichLib);
	if(logFile) fclose(logFile);
}

//-------------------------------------------------------------------------

void Analyzer_SetOpcode(WORD opcode, bool s2c)
{
	// Gelen paket opcode'unu kaydet
	packet_opcode = opcode;

	static char msg[131072] = {0};
	packet_show = false;

	// 1. Yön kontrolü: Server->Client veya Client->Server gösterilecek mi?
	bool directionEnabled = s2c ? bShowS2C : bShowC2S;
	
	// 2. "Sadece göster" listesi kontrolü (boşsa veya opcode listede varsa geçer)
	bool isInAllowList = (showOnlyOpcodes.size() == 0) || 
	                     (std::find(showOnlyOpcodes.begin(), showOnlyOpcodes.end(), packet_opcode) != showOnlyOpcodes.end());
	
	// 3. "Filtrele/Yoksay" listesi kontrolü (opcode listede YOKSA geçer)
	bool isNotInIgnoreList = (std::find(filterOpcodes.begin(), filterOpcodes.end(), packet_opcode) == filterOpcodes.end());
	
	// 4. Özel durum: "Sadece göster" listesinde özellikle belirtilmiş mi?
	bool isPriorityOpcode = std::find(showOnlyOpcodes.begin(), showOnlyOpcodes.end(), packet_opcode) != showOnlyOpcodes.end();
	
	// Gösterme koşulu: (Normal filtrelerden geçerse) VEYA (Öncelikli opcode ise)
	if((directionEnabled && isInAllowList && isNotInIgnoreList && !isPriorityOpcode) || isPriorityOpcode)
	{
		packet_show = true;
	}

	// Paket başlığını hazırla (0x prefix + opcode ismi ile)
	const char* opcodeName = GetOpcodeName(packet_opcode);
	if(s2c)
		_snprintf(msg, 1023, "[Server -> Client][0x%.4X - %s]\r\n", packet_opcode, opcodeName);
	else
		_snprintf(msg, 1023, "[Client -> Server][0x%.4X - %s]\r\n", packet_opcode, opcodeName);
	AppendText(packet_show, msg);
}

//-------------------------------------------------------------------------

void Analyzer_StreamData(LPBYTE stream, DWORD count_)
{
	static char msg[131072] = {0};

	int outputsize = count_;
	if(outputsize % 16 != 0) outputsize += (16 - outputsize % 16);
	if(outputsize == 0) outputsize = 16;
	int ctr = 0;
	char ch[17] = {0};
	int x1 = 0;
	for(int x = 0; x < outputsize; ++x)
	{
		BYTE b;
		if(x < (int)count_)
		{
			b = stream[x];
			sprintf(msg + ctr, "%.2X ", b);
			ch[x1] = (isprint(b) && !isspace(b) ? b : '.');
		}
		else
		{
			sprintf(msg + ctr, "   ");
			ch[x1] = '.';
		}
		x1++;
		ctr += 3;
		if((x+1) % 16 == 0)
		{
			x1 = 0;
			sprintf(msg + ctr, "  %s", ch);
			ctr += 18;
			sprintf(msg + ctr, "\r\n");
			ctr += 2;
		}
	}
	//strcat(msg, "\n");
	AppendText(packet_show, msg);
}

//-------------------------------------------------------------------------

void Analyzer_EndPacket()
{
	AppendText(packet_show, "\r\n");
}

//-------------------------------------------------------------------------

// Main GUI thread
DWORD WINAPI GuiThread(LPVOID lpParam)
{
	HWND hwnd = CreateDialog(globalInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
	MSG Msg = {0};
	while(GetMessage(&Msg, NULL, 0, 0) > 0)
	{
		if(!IsDialogMessage(hwnd, &Msg))
		{
			TranslateMessage(&Msg);
			DispatchMessage(&Msg);
		}
	}
	return 0;
}

//-----------------------------------------------------------------------------

// Window procedure for the GUI
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// Handle the message
	switch(uMsg) 
	{
		// Dialog initialize
		case WM_INITDIALOG:
		{
			// Store the gui window
			hGui = hWnd;

			// Load the dll
			hRichLib = LoadLibraryA("RICHED32.DLL");

			// Create a font
			hFont = CreateFontA(16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, "Courier New");

			// Create the log dir and file
			char filename[MAX_PATH + 1] = {0};
			std::string fnstr = GetCommonDirectory(base_dir);
			fnstr += "analyzer";
			CreateDirectoryA(fnstr.c_str(), 0);
			std::string fnstr2 = fnstr;
			fnstr += "\\log\\";
			CreateDirectoryA(fnstr.c_str(), 0);
			fnstr2 += "\\snippets\\";
			CreateDirectoryA(fnstr2.c_str(), 0);
			_snprintf(filename, MAX_PATH, "%s\\%i.txt", fnstr.c_str(), GetTickCount());
			logFile = fopen(filename, "w");
			if(logFile == NULL)
			{
				MessageBoxA(0, "Could not create the logging file.", "Error", MB_ICONERROR);
				ExitProcess(0);
			}

			RECT r = {0};
			ShowWindow(GetDlgItem(hWnd, IDC_LOG_AREA), SW_HIDE);
			GetClientRect(GetDlgItem(hWnd, IDC_LOG_AREA), &r);
			hEdit = CreateWindowA("RichEdit", "", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL | ES_NOHIDESEL, 0, 0, r.right, r.bottom, hWnd, NULL, GetModuleHandle(0), NULL);
			if(!hEdit)
			{
				MessageBoxA(hWnd, "Richedit creation failed", "Error", MB_OK | MB_ICONEXCLAMATION);
				return FALSE;
			}

			// Set a fixed width font
			SendMessage(hEdit, WM_SETFONT, (WPARAM)hFont, TRUE);

			// Load the filter
			LoadFilter();

			// Clear out the list boxes
			SendMessage(GetDlgItem(hWnd, IDC_IGNORE_LIST), LB_RESETCONTENT, 0, 0);
			SendMessage(GetDlgItem(hWnd, IDC_ALLOW_LIST), LB_RESETCONTENT, 0, 0);
			SendMessage(GetDlgItem(hWnd, IDC_CLIENT_VIEW), LB_RESETCONTENT, 0, 0);

			// update check boxes with the value from the ini
			Button_SetCheck(GetDlgItem(hWnd, IDC_C2S), GetCtoS());
			Button_SetCheck(GetDlgItem(hWnd, IDC_S2C), GetStoC());

			// Default
			Button_SetCheck(GetDlgItem(hWnd, IDC_CLIENT_ON), BST_CHECKED);

			// Load ignore opcodes
			std::vector<DWORD> filterOpcodes = GetIgnoredOpcodes();
			for(size_t x = 0; x < filterOpcodes.size(); ++x)
			{
				char tmp[256] = {0};
				_snprintf(tmp, 255, "%X", filterOpcodes[x]);
				SendMessage(GetDlgItem(hWnd, IDC_IGNORE_LIST), LB_ADDSTRING, 0, (LPARAM)tmp);
			}

			// Load allow opcodes
			std::vector<DWORD> showOnlyOpcodes = GetAllowedOpcodes();
			for(size_t x = 0; x < showOnlyOpcodes.size(); ++x)
			{
				char tmp[256] = {0};
				_snprintf(tmp, 255, "%X", showOnlyOpcodes[x]);
				SendMessage(GetDlgItem(hWnd, IDC_ALLOW_LIST), LB_ADDSTRING, 0, (LPARAM)tmp);
			}

			// 4 characters max
			SendMessage(GetDlgItem(hWnd, IDC_IGNORE_OPCODE), EM_LIMITTEXT, 4, 0);
			SendMessage(GetDlgItem(hWnd, IDC_ALLOW_OPCODE), EM_LIMITTEXT, 4, 0);

			// Default setting
			Button_SetCheck(GetDlgItem(hWnd, IDC_FILELOG), bDoLog);
		}
		break;

		// Commands
		case WM_COMMAND:
		{
			int button = LOWORD(wParam);
			switch (button)
			{
				// Exit
				case IDCANCEL:
				{
					ShowWindow(hWnd, SW_MINIMIZE);
				} break;

				//-------------------------------------------------------------------------

				// Clear the view
				case IDC_VIEW_CLEAR:
				{
					SetWindowTextA(hEdit, "");
				} break;

				// Save the view
				case IDC_VIEW_SAVE:
				{
					// Get the total characters in the view
					int length = GetWindowTextLength(hEdit);

					// Allocate memory for it
					char * buffer = new char[length + 1];

					// Clear out the buffer
					memset(buffer, 0, length + 1);

					// Get the contents
					GetWindowTextA(hEdit, buffer, length + 1);

					// Filename to save to
					char fn[261] = {0};

					// Build the filename

					std::string fnstr = base_dir;
					fnstr += "\\analyzer\\snippets\\";
					fnstr = GetCommonDirectory(fnstr);
					_snprintf(fn, 260, "%s\\%i.log", fnstr.c_str(), GetTickCount());

					// Create the file
					FILE * outFile = fopen(fn, "w");

					// Make sure it was created
					if(outFile)
					{
						// Save the data and close the file
						fprintf(outFile, "%s", buffer);
						fclose(outFile);
					}
					else
					{
						MessageBoxA(0, "Could not save the snippet.", "Error", MB_ICONERROR);
					}

					// Free the memory
					delete [] buffer;
				} break;

				//-------------------------------------------------------------------------
				// Do not show opcodes

				// Add opcode
				case IDC_IGNORE_ADD:
				{
					char tmp[8] = {0};
					GetWindowTextA(GetDlgItem(hWnd, IDC_IGNORE_OPCODE), tmp, 5);
					int length = GetWindowTextLength(GetDlgItem(hWnd, IDC_IGNORE_OPCODE));

					// Support 3 and 4 byte opcodes, do not think there are 2 or 1 byte opcodes :P
					if(length == 4) if(!isalnum(tmp[0]) || !isalnum(tmp[1]) || !isalnum(tmp[2]) || !isalnum(tmp[3])) break;
					if(length == 3) if(!isalnum(tmp[0]) || !isalnum(tmp[1]) || !isalnum(tmp[2])) break;
					if(length < 3) break;

					if(AddIgnoreOpcode(HexStringToInteger<DWORD>(tmp)))
					{
						SendMessage(GetDlgItem(hWnd, IDC_IGNORE_LIST), LB_ADDSTRING, 0, (LPARAM)tmp);
					}
					SetWindowTextA(GetDlgItem(hWnd, IDC_IGNORE_OPCODE), "");
				} break;

				// Remove opcode
				case IDC_IGNORE_REMOVE:
				{
					LRESULT sel = SendMessage(GetDlgItem(hWnd, IDC_IGNORE_LIST), LB_GETCURSEL, 0, 0);
					if(sel != -1)
					{
						char tmp[1024] = {0};
						SendMessage(GetDlgItem(hWnd, IDC_IGNORE_LIST), LB_GETTEXT, sel, (LPARAM)tmp);
						DWORD opcode = HexStringToInteger<DWORD>(tmp);
						RemoveIgnoreOpcode(opcode);
						SendMessage(GetDlgItem(hWnd, IDC_IGNORE_LIST), LB_DELETESTRING, sel, 0);
					}
				} break;

				// Clear all
				case IDC_IGNORE_RESET:
				{
					if(MessageBoxA(0, "Are you sure you wish to clear the \"Do Not Show Opcodes\" list?", "Confirmation", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
					{
						SendMessage(GetDlgItem(hWnd, IDC_IGNORE_LIST), LB_RESETCONTENT, 0, 0);
						ClearIgnoreOpcodes();
					}
				} break;

				//-------------------------------------------------------------------------
				// show only opcodes

				// Add opcode
				case IDC_ALLOW_ADD:
				{
					char tmp[8] = {0};
					GetWindowTextA(GetDlgItem(hWnd, IDC_ALLOW_OPCODE), tmp, 5);
					int length = GetWindowTextLength(GetDlgItem(hWnd, IDC_ALLOW_OPCODE));

					// Support 3 and 4 byte opcodes, do not think there are 2 or 1 byte opcodes :P
					if(length == 4) if(!isalnum(tmp[0]) || !isalnum(tmp[1]) || !isalnum(tmp[2]) || !isalnum(tmp[3])) break;
					if(length == 3) if(!isalnum(tmp[0]) || !isalnum(tmp[1]) || !isalnum(tmp[2])) break;
					if(length < 3) break;

					if(AddAllowOpcode(HexStringToInteger<DWORD>(tmp)))
					{
						SendMessage(GetDlgItem(hWnd, IDC_ALLOW_LIST), LB_ADDSTRING, 0, (LPARAM)tmp);
					}
					SetWindowTextA(GetDlgItem(hWnd, IDC_ALLOW_OPCODE), "");
				} break;

				// Remove opcode
				case IDC_ALLOW_REMOVE:
				{
					LRESULT sel = SendMessage(GetDlgItem(hWnd, IDC_ALLOW_LIST), LB_GETCURSEL, 0, 0);
					if(sel != -1)
					{
						char tmp[1024] = {0};
						SendMessage(GetDlgItem(hWnd, IDC_ALLOW_LIST), LB_GETTEXT, sel, (LPARAM)tmp);
						DWORD opcode = HexStringToInteger<DWORD>(tmp);
						RemoveAllowOpcode(opcode);
						SendMessage(GetDlgItem(hWnd, IDC_ALLOW_LIST), LB_DELETESTRING, sel, 0);
					}
				} break;

				// Clear all
				case IDC_ALLOW_RESET:
				{
					if(MessageBoxA(0, "Are you sure you wish to clear the \"Show Only Opcodes\" list?", "Confirmation", MB_ICONQUESTION | MB_YESNO | MB_DEFBUTTON2) == IDYES)
					{
						SendMessage(GetDlgItem(hWnd, IDC_ALLOW_LIST), LB_RESETCONTENT, 0, 0);
						ClearAllowOpcodes();
					}
				} break;

				//-------------------------------------------------------------------------
				//  General filtering

				// Toggle destinations
				case IDC_S2C:
				{
					ToggleStoC();
				} break;

				// Toggle destinations
				case IDC_C2S:
				{
					ToggleCtoS();
				} break;

				//-------------------------------------------------------------------------

				// Should logging be done
				case IDC_FILELOG:
				{
					bDoLog = Button_GetCheck(GetDlgItem(hWnd, IDC_FILELOG));
				} break;

				//-------------------------------------------------------------------------

				// Enable logging for a selected client
				case IDC_CLIENT_ON:
				case IDC_CLIENT_OFF:
				{
					// Enable logging for this client
					if(button == IDC_CLIENT_ON)
						bDoLogging = true;
					else
						bDoLogging = false;

				} break;

				//-------------------------------------------------------------------------

				case IDC_SHOW_DIR:
				{
					std::string path = GetCommonDirectory("edxSilkroadLoader5");
					ShellExecuteA(hWnd, "open", path.c_str(), NULL, NULL, SW_NORMAL);
				} break;
			}
		} break;

		default:
		{
			// Message not handled
			return FALSE;
		}
	}

	// Message handled
	return TRUE;
}

//-----------------------------------------------------------------------------

// Appends text to the rich edit view
void AppendText(bool doShow, char *cText)
{
	if(logFile && bDoLog) { fprintf(logFile, "%s\n", cText); fflush(logFile); }
	if(doShow && bDoLogging)
	{
		if(!hEdit) return;
		CHARRANGE cr = {-1, -1};
		SendMessage(hEdit, EM_EXSETSEL, 0,(LPARAM)&cr);
		SendMessage(hEdit, EM_REPLACESEL, 0, (LPARAM)cText);
		SendMessage(hEdit, WM_VSCROLL, /*SB_LINEDOWN*/ SB_BOTTOM, (LPARAM)NULL);
	}
}

//-----------------------------------------------------------------------------

// Load the settings
void LoadFilter()
{
	filterOpcodes.clear();
	showOnlyOpcodes.clear();
	DWORD tmp = 0;
	DWORD tmp2 = 0;
	std::string fn = base_dir;
	fn += "\\analyzer\\";
	fn = GetCommonDirectory(fn);
	fn += "filter.ini";
	std::ifstream inFile(fn.c_str());
	if(inFile.is_open() == false)
		return;
	inFile >> bShowC2S;
	inFile >> bShowS2C;
	inFile >> tmp;
	for(size_t x = 0; x < tmp; ++x)
	{
		inFile >> tmp2;
		filterOpcodes.push_back(tmp2);
	}
	inFile >> tmp;
	for(size_t x = 0; x < tmp; ++x)
	{
		inFile >> tmp2;
		showOnlyOpcodes.push_back(tmp2);
	}
	inFile.close();
}

//-----------------------------------------------------------------------------

// Save the settings
void SaveFilter()
{
	std::string fn = base_dir;
	fn += "\\analyzer\\";
	fn = GetCommonDirectory(fn);
	fn += "filter.ini";
	std::ofstream outFile(fn.c_str());
	if(outFile.is_open() == false)
	{
		MessageBoxA(0, "Could not save the analyzer filter.", "Error", MB_ICONERROR);
		return;
	}
	outFile << bShowC2S << std::endl;
	outFile << bShowS2C << std::endl;
	outFile << (int)filterOpcodes.size() << std::endl;
	for(size_t x = 0; x < filterOpcodes.size(); ++x)
	{
		outFile << filterOpcodes[x] << std::endl;
	}
	outFile << (int)(showOnlyOpcodes.size()) << std::endl;
	for(size_t x = 0; x < showOnlyOpcodes.size(); ++x)
	{
		outFile << showOnlyOpcodes[x] << std::endl;
	}
	outFile.close();
}

//-----------------------------------------------------------------------------

// Add an ignore opcode
bool AddIgnoreOpcode(DWORD opcode)
{
	if(std::find(filterOpcodes.begin(), filterOpcodes.end(), opcode) == filterOpcodes.end())
	{
		filterOpcodes.push_back(opcode);
		SaveFilter();
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------

// Remove an ignore opcode
void RemoveIgnoreOpcode(DWORD opcode)
{
	std::vector<DWORD>::iterator itr = std::find(filterOpcodes.begin(), filterOpcodes.end(), opcode);
	if(itr != filterOpcodes.end())
	{
		filterOpcodes.erase(itr);
		SaveFilter();
	}
}

//-----------------------------------------------------------------------------

// Reset the ignore opcode list
void ClearIgnoreOpcodes()
{
	filterOpcodes.clear();
	SaveFilter();
}

//-----------------------------------------------------------------------------

// Add a show only opcode
bool AddAllowOpcode(DWORD opcode)
{
	if(std::find(showOnlyOpcodes.begin(), showOnlyOpcodes.end(), opcode) == showOnlyOpcodes.end())
	{
		showOnlyOpcodes.push_back(opcode);
		SaveFilter();
		return true;
	}
	return false;
}

//-----------------------------------------------------------------------------

// Remove a show only opcode
void RemoveAllowOpcode(DWORD opcode)
{
	std::vector<DWORD>::iterator itr = std::find(showOnlyOpcodes.begin(), showOnlyOpcodes.end(), opcode);
	if(itr != showOnlyOpcodes.end())
	{
		showOnlyOpcodes.erase(itr);
		SaveFilter();
	}
}

//-----------------------------------------------------------------------------

// Reset the show only opcode list
void ClearAllowOpcodes()
{
	showOnlyOpcodes.clear();
	SaveFilter();
}

//-----------------------------------------------------------------------------

// Destination filtering
void ShowCtoS(bool show)
{
	bShowC2S = show;
	SaveFilter();
}

//-----------------------------------------------------------------------------

// Destination filtering
void ShowStoC(bool show)
{
	bShowS2C = show;
	SaveFilter();
}

//-----------------------------------------------------------------------------

// Toggle client to server packet logging
void ToggleCtoS()
{
	bShowC2S = !bShowC2S;
	SaveFilter();
}

//-----------------------------------------------------------------------------

// Toggle server to client packet logging
void ToggleStoC()
{
	bShowS2C = !bShowS2C;
	SaveFilter();
}

//-----------------------------------------------------------------------------

// Return if the server to client packets are logged
BOOL GetStoC()
{
	return bShowS2C;
}

//-----------------------------------------------------------------------------

// Return if the client to server packets are logged
BOOL GetCtoS()
{
	return bShowC2S;
}

//-----------------------------------------------------------------------------

// Get lists of opcodes
std::vector<DWORD> GetAllowedOpcodes()
{
	return showOnlyOpcodes;
}

//-----------------------------------------------------------------------------

// Get lists of opcodes
std::vector<DWORD> GetIgnoredOpcodes()
{
	return filterOpcodes;
}

//-----------------------------------------------------------------------------




























