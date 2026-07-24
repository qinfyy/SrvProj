#include "GatewayController.h"
#include "../AccountServer.h"
#include <sstream>
#include <string_view>
#include <openssl/rand.h>
#include "../GameSession.h"
#include "../AeadTool.h"
#include "../GameServices.h"
#include "../logger.h"
#include "../proto/NetMsgId.h"
#include "Login.h"
#include "Activity.h"
#include "Quest.h"
#include "Story.h"
#include "PlayerHandler.h"
#include "Mail.h"
#include "Gacha.h"
#include "Mall.h"
#include "Item.h"
#include "Character.h"
#include "Disc.h"
#include "BattlePass.h"
#include "Tower.h"
#include "../GameTime.h"

typedef std::string(*ReqHandler)(GameSession*, const std::string&);

std::unordered_map<short, ReqHandler> g_HandlerMap = {
    {ike_req, ike_req__Handler},
    {player_login_req, player_login_req__Handler},
    {player_data_req, player_data_req__Handler},
    {player_reg_req, player_reg_req__Handler},
    {player_ping_req, player_ping_req__Handler},
    {player_formation_req, player_formation_req__Handler},
    {player_signature_edit_req, player_signature_edit_req__Handler},
    {player_learn_req, player_learn_req__Handler},
    {energy_info_req, energy_info_req__Handler},
    {mall_gem_list_req, mall_gem_list_req__Handler},
    {mall_gem_order_req, mall_gem_order_req__Handler},
    {mall_order_cancel_req, mall_order_cancel_req__Handler},
    {mall_order_collect_req, mall_order_collect_req__Handler},
    {mall_monthlyCard_list_req, mall_monthlyCard_list_req__Handler},
    {mall_monthlyCard_order_req, mall_monthlyCard_order_req__Handler},
    {mall_package_list_req, mall_package_list_req__Handler},
    {mall_package_order_req, mall_package_order_req__Handler},
    {mall_shop_list_req, mall_shop_list_req__Handler},
    {mall_shop_order_req, mall_shop_order_req__Handler},
    {gem_convert_req, gem_convert_req__Handler},
    {item_use_req, item_use_req__Handler},
    {item_product_req, item_product_req__Handler},
    {item_quick_growth_req, item_quick_growth_req__Handler},
    {char_upgrade_req, char_upgrade_req__Handler},
    {char_advance_req, char_advance_req__Handler},
    {char_skill_upgrade_req, char_skill_upgrade_req__Handler},
    {char_skin_set_req, char_skin_set_req__Handler},
    {char_affinity_gift_send_req, char_affinity_gift_send_req__Handler},
    {char_favorite_set_req, char_favorite_set_req__Handler},
    {char_gem_use_preset_req, char_gem_use_preset_req__Handler},
    {char_gem_equip_gem_req, char_gem_equip_gem_req__Handler},
    {char_gem_refresh_req, char_gem_refresh_req__Handler},
    {char_gem_replace_attribute_req, char_gem_replace_attribute_req__Handler},
    {char_gem_update_gem_lock_status_req, char_gem_update_gem_lock_status_req__Handler},
    {char_gem_overlock_req, char_gem_overlock_req__Handler},
    {disc_strengthen_req, disc_strengthen_req__Handler},
    {disc_promote_req, disc_promote_req__Handler},
    {disc_limit_break_req, disc_limit_break_req__Handler},
    {disc_read_reward_receive_req, disc_read_reward_receive_req__Handler},
    {disc_all_limit_break_req, disc_all_limit_break_req__Handler},
    {activity_detail_req, activity_detail_req__Handler},
    {potential_preselection_list_req, potential_preselection_list_req__Handler},
    {potential_preselection_import_req, potential_preselection_import_req__Handler},
    {potential_preselection_name_set_req, potential_preselection_name_set_req__Handler},
    {potential_preselection_preference_set_req, potential_preselection_preference_set_req__Handler},
    {potential_preselection_update_req, potential_preselection_update_req__Handler},
    {potential_preselection_delete_req, potential_preselection_delete_req__Handler},
    {daily_shop_reward_receive_req, daily_shop_reward_receive_req__Handler},
    {daily_mall_reward_receive_req, daily_mall_reward_receive_req__Handler},
    {quest_daily_reward_receive_req, quest_daily_reward_receive_req__Handler},
    {quest_daily_active_reward_receive_req, quest_daily_active_reward_receive_req__Handler},
    {quest_weekly_reward_receive_req, quest_weekly_reward_receive_req__Handler},
    {quest_weekly_active_reward_receive_req, quest_weekly_active_reward_receive_req__Handler},
    {battle_pass_info_req, battle_pass_info_req__Handler},
    {battle_pass_reward_receive_req, battle_pass_reward_receive_req__Handler},
    {battle_pass_level_buy_req, battle_pass_level_buy_req__Handler},
    {battle_pass_order_req, battle_pass_order_req__Handler},
    {battle_pass_order_collect_req, battle_pass_order_collect_req__Handler},
    {battle_pass_quest_reward_receive_req, battle_pass_quest_reward_receive_req__Handler},
    {achievement_info_req, achievement_info_req__Handler},
    {achievement_reward_receive_req, achievement_reward_receive_req__Handler},
    {client_event_report_req, client_event_report_req__Handler},
    {plot_reward_receive_req, plot_reward_receive_req__Handler},
    {story_apply_req, story_apply_req__Handler},
    {story_settle_req, story_settle_req__Handler},
    {story_set_info_req, story_set_info_req__Handler},
    {story_set_reward_receive_req, story_set_reward_receive_req__Handler},
    {mail_list_req, mail_list_req__Handler},
    {mail_read_req, mail_read_req__Handler},
    {mail_recv_req, mail_recv_req__Handler},
    {mail_remove_req, mail_remove_req__Handler},
    {mail_pin_req, mail_pin_req__Handler},
    {gacha_spin_req, gacha_spin_req__Handler},
    {gacha_information_req, gacha_information_req__Handler},
    {gacha_histories_req, gacha_histories_req__Handler},
    {gacha_guarantee_reward_receive_req, gacha_guarantee_reward_receive_req__Handler},
    {gacha_newbie_spin_req, gacha_newbie_spin_req__Handler},
    {gacha_newbie_save_req, gacha_newbie_save_req__Handler},
    {gacha_newbie_obtain_req, gacha_newbie_obtain_req__Handler},
    {gacha_newbie_info_req, gacha_newbie_info_req__Handler},
    {star_tower_build_brief_list_get_req, star_tower_build_brief_list_get_req__Handler},
    {star_tower_apply_req, star_tower_apply_req__Handler},
    {star_tower_build_delete_req, star_tower_build_delete_req__Handler},
    {star_tower_build_detail_get_req, star_tower_build_detail_get_req__Handler},
    {star_tower_build_lock_unlock_req, star_tower_build_lock_unlock_req__Handler},
    {star_tower_build_name_set_req, star_tower_build_name_set_req__Handler},
    {star_tower_build_preference_set_req, star_tower_build_preference_set_req__Handler},
    {star_tower_build_whether_save_req, star_tower_build_whether_save_req__Handler},
    {star_tower_give_up_req, star_tower_give_up_req__Handler},
    {star_tower_info_req, star_tower_info_req__Handler},
    {star_tower_interact_req, star_tower_interact_req__Handler},
    {star_tower_book_potential_brief_list_get_req, star_tower_book_potential_brief_list_get_req__Handler},
    {star_tower_book_char_potential_get_req, star_tower_book_char_potential_get_req__Handler},
    {star_tower_book_potential_reward_receive_req, star_tower_book_potential_reward_receive_req__Handler},
    {star_tower_book_event_reward_receive_req, star_tower_book_event_reward_receive_req__Handler},
    {tower_book_fate_card_detail_req, tower_book_fate_card_detail_req__Handler},
    {tower_book_fate_card_reward_receive_req, tower_book_fate_card_reward_receive_req__Handler},
    {npc_affinity_book_get_req, npc_affinity_book_get_req__Handler},
    {npc_affinity_plot_reward_receive_req, npc_affinity_plot_reward_receive_req__Handler},
    {tower_growth_detail_req, tower_growth_detail_req__Handler},
    {tower_growth_group_node_unlock_req, tower_growth_group_node_unlock_req__Handler},
    {tower_growth_node_unlock_req, tower_growth_node_unlock_req__Handler},
};

AsyncTask<void> AgentHandler(RouteContext& context, const HttpRequest& request, HttpResponseWriter& writer)
{
    HttpResponse response;
    co_await context.Runtime().RunBlocking([&request, &response]
        {
            const HttpRequest& req = request;
            HttpResponse& rsp = response;
            GameSession* session = nullptr;

            std::string defaultSessionKey = AeadTool::twServerGarbleKey;
            bool hasKey3 = false;
            int encryptFunction = 0;

            std::string sessionToken;
            if (auto it = req.headers.find("X-Token"); it != req.headers.end()) {
                sessionToken = it->second;
            }

            rsp.headers["Server"] = "agent";

            if (!sessionToken.empty()) {
                session = GameServices::Instance().GetSessionByToken(sessionToken);

                // 找不到会话
                if (!session || session->mKey.empty()) {
                    rsp.statusCode = 500;
                    rsp.body = "";
                    return;
                }

                defaultSessionKey = session->mKey;
                encryptFunction = session->mEncryptFunction;
                hasKey3 = true;
            }

            std::string reqData;
            short msgId = 0;

            try {
                std::string body = req.body;
                std::string plain;
                int offset = 0;
                if (body.size() <= 12) {
                    rsp.statusCode = 500;
                    rsp.body = "";
                    return;
                }

                if (hasKey3) {
                    std::array<char, 12> iv;
                    memcpy(iv.data(), body.data(), 12);
                    std::string cipher(reinterpret_cast<const char*>(body.data() + 12), body.size() - 12);
                    AeadTool::Dencrypt_BouncyCastle(plain, defaultSessionKey, std::string_view(iv.data(), iv.size()), cipher, cipher.size(), true, encryptFunction);
                    offset = 10; // 跳过客户端包包头
                }
                else {
                    auto whitewashed = AeadUtil::Wash(body, defaultSessionKey);
                    std::array<char, 12> iv;
                    memcpy(iv.data(), whitewashed.data(), 12);
                    std::string cipher(reinterpret_cast<const char*>(whitewashed.data() + 12), whitewashed.size() - 12);
                    AeadTool::Dencrypt_BouncyCastle(plain, defaultSessionKey, std::string_view(iv.data(), iv.size()), cipher, cipher.size(), true, 0); // 0 = AES GCM, 1 = ChaCha20Poly1305
                }

                msgId = (static_cast<uint8_t>(plain[offset]) << 8) | (static_cast<uint8_t>(plain[offset + 1]));
                offset += 2;
                reqData = plain.substr(offset);

                LOG_DEBUG("Received request, msgId: {}, data size: {}, sessionToken: {}, hasKey3: {}, encryptFunction: {}",
                    msgId, reqData.size(), sessionToken, hasKey3, encryptFunction);

                if (session) {
                    session->mLastActiveTime = GameTime::NowMilliseconds();
                }
                GameServices::Instance().CleanupExpiredSessions();

                ReqHandler handler = nullptr;
                if (auto it = g_HandlerMap.find(msgId); it != g_HandlerMap.end())
                {
                    handler = it->second;
                }

                std::string rspOut;
                if (handler) {
                    rspOut = handler(session, reqData);
                }
                else {
                    rspOut = DummyHandler(msgId);
                }

                if (rspOut.empty()) {
                    LOG_WARNING("Unhandled request: {}", msgId);
                    rsp.statusCode = 500;
                    rsp.body = "";
                    return;
                }
                else {
                    LOG_DEBUG("Request handled successfully, response size: {}", rspOut.size());
                }

                if (hasKey3) {
                    std::array<char, 12> iv;
                    RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv.size());
                    std::string cipher;
                    AeadTool::Encrypt_BouncyCastle(cipher, defaultSessionKey, std::string_view(iv.data(), iv.size()), rspOut, static_cast<int>(rspOut.size()), true, encryptFunction);

                    std::string finalResult;
                    finalResult.reserve(cipher.size() + 12);
                    finalResult.append(iv.data(), 12);
                    finalResult.append(cipher);

                    rsp.statusCode = 200;
                    rsp.body = finalResult;
                }
                else {
                    std::array<char, 12> iv;
                    RAND_bytes(reinterpret_cast<unsigned char*>(iv.data()), iv.size());
                    std::string cipher;
                    AeadTool::Encrypt_BouncyCastle(cipher, defaultSessionKey, std::string_view(iv.data(), iv.size()), rspOut, static_cast<int>(rspOut.size()), true, 0); // 0 = AES GCM, 1 = ChaCha20Poly1305

                    std::string result;
                    result.reserve(cipher.size() + 12);
                    result.append(iv.data(), 12);
                    result.append(cipher);

                    auto finalResult = AeadUtil::Obfuscate(result, defaultSessionKey);

                    rsp.statusCode = 200;
                    rsp.body = finalResult;

                    return;
                }
            }
            catch (const std::exception& e) {
                // error
                std::ostringstream logOs;
                logOs << "Agent 错误: ";
                const std::exception* current = &e;
                int level = 0;

                while (current) {
                    logOs << std::string(level * 2, ' ') << current->what() << std::endl;

                    try {
                        std::rethrow_if_nested(*current);
                        break;
                    }
                    catch (const std::exception& nested) {
                        current = &nested;
                        level++;
                    }
                    catch (...) {
                        break;
                    }
                }

                LOG_ERROR(logOs.str());

                rsp.statusCode = 500;
                rsp.body = "";

                return;
            }
        });
    co_await writer.WriteResponse(response);
}

std::string DummyHandler(short reqId)
{
    std::string reqName;
    for (const auto& [name, id] : kNetMsgIdMap) {
        if (id == reqId) {
            reqName = name;
            break;
        }
    }
    if (reqName.empty()) {
        return "";
    }

    constexpr std::string_view suffix = "req";
    if (!std::string_view(reqName).ends_with(suffix)) {
        return "";
    }

    std::string failedAckName = reqName.substr(0, reqName.size() - suffix.size()) + "failed_ack";
    const auto failedAckIt = kNetMsgIdMap.find(failedAckName);
    if (failedAckIt == kNetMsgIdMap.end()) {
        return "";
    }

    return GameSession::EncodeMessage(static_cast<short>(failedAckIt->second), "");
}
