#include "PaymentController.h"

#include <nlohmann/json.hpp>

#include "Config.h"
#include "GameTime.h"
#include "Logger.h"
#include "ResultCode.h"
#include "Util.h"

namespace
{
    const std::string kOrderProductsResponse = U8("{\"Code\":200,\"Data\":{\"List\":[{\"ID\":\"3742098088\",\"Name\":\"希娅_养成礼包\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.02_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742162634\",\"Name\":\"75 星之彩\",\"Price\":33,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina75\",\"GameProductID\":\"gem.tier7\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742180328\",\"Name\":\"千都世_角色资源礼包\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.01_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742190926\",\"Name\":\"1015 星之彩\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina1015\",\"GameProductID\":\"gem.tier4\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742197373\",\"Name\":\"每周_角色资源礼包\",\"Price\":190,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_w\",\"GameProductID\":\"pack.01_role_w\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742208918\",\"Name\":\"皮肤_98\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.01\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742267540\",\"Name\":\"希娅_pu星盘券礼包\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.02_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742268311\",\"Name\":\"490 星之彩\",\"Price\":190,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina490\",\"GameProductID\":\"gem.tier5\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742291527\",\"Name\":\"每月_pu角色券礼包\",\"Price\":600,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_m\",\"GameProductID\":\"pack.01_role_m\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742292395\",\"Name\":\"希娅_pu角色券礼包\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.02_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742319192\",\"Name\":\"8500 星之彩\",\"Price\":3000,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina8500\",\"GameProductID\":\"gem.tier1\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742392240\",\"Name\":\"新手_SR角色自选礼包\",\"Price\":150,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.sr\",\"GameProductID\":\"pack.sr\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742392588\",\"Name\":\"开服_pu星盘券礼包\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.op_disc\",\"GameProductID\":\"pack.op_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742398399\",\"Name\":\"希娅_礼物礼包\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.02_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742410933\",\"Name\":\"98_BP\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.battlepass.98\",\"GameProductID\":\"battlepass.98\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742421531\",\"Name\":\"千都世_礼物礼包\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.01_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742464254\",\"Name\":\"每月_pu星盘券礼包\",\"Price\":600,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_m\",\"GameProductID\":\"pack.01_disc_m\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742528647\",\"Name\":\"新手_pu角色券礼包\",\"Price\":340,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role\",\"GameProductID\":\"pack.role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742542814\",\"Name\":\"新手_6元破冰礼包\",\"Price\":33,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.first\",\"GameProductID\":\"pack.first\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742543170\",\"Name\":\"4300 星之彩\",\"Price\":1600,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina4300\",\"GameProductID\":\"gem.tier2\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742556595\",\"Name\":\"每周_星盘资源礼包\",\"Price\":190,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_w\",\"GameProductID\":\"pack.01_disc_w\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742577471\",\"Name\":\"开服_pu角色券礼包\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.op_role\",\"GameProductID\":\"pack.op_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742639592\",\"Name\":\"新手_pu星盘券礼包\",\"Price\":340,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc\",\"GameProductID\":\"pack.disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742663664\",\"Name\":\"月卡\",\"Price\":150,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.monthlycard.small\",\"GameProductID\":\"monthlyCard.small\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742807914\",\"Name\":\"2200 星之彩\",\"Price\":840,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina2200\",\"GameProductID\":\"gem.tier3\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742822198\",\"Name\":\"230 星之彩\",\"Price\":90,\"Desc\":\"\",\"StoreProductID\":\"com.yostar.stellasora.stellanitelumina230\",\"GameProductID\":\"gem.tier6\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742909739\",\"Name\":\"新手_普池角色券礼包\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_common\",\"GameProductID\":\"pack.role_common\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742925108\",\"Name\":\"68_BP\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.battlepass.58\",\"GameProductID\":\"battlepass.58\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"3742929455\",\"Name\":\"38_BP\",\"Price\":250,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.battlepass.50\",\"GameProductID\":\"battlepass.50\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4700156687\",\"Name\":\"新芽绽放·灿金闪碟组合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.03_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4700329320\",\"Name\":\"新芽绽放·旅人养成组合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.03_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4700366779\",\"Name\":\"新芽绽放·倾心礼品组合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.03_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4700546204\",\"Name\":\"新芽绽放·青空礼券组合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.03_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4700590251\",\"Name\":\"异国风起时\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.02\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4701124771\",\"Name\":\"搖曳輕風·青空禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.04_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4701253972\",\"Name\":\"搖曳輕風·旅人養成組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.04_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4701651218\",\"Name\":\"搖曳輕風·傾心禮品組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.04_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4701795228\",\"Name\":\"搖曳輕風·燦金閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.04_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4938079688\",\"Name\":\"雪願閃耀·超值禮券組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_s\",\"GameProductID\":\"pack.05_role_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4938110493\",\"Name\":\"雪願閃耀·豪華閃碟組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_l\",\"GameProductID\":\"pack.05_disc_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4938355895\",\"Name\":\"雪願閃耀·傾心禮品組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.05_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4938439847\",\"Name\":\"雪願閃耀·特惠閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.05_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4938468992\",\"Name\":\"雪願閃耀·特惠禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.05_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4938572651\",\"Name\":\"雪願閃耀·豪華禮券組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_l\",\"GameProductID\":\"pack.05_role_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4938651475\",\"Name\":\"雪願閃耀·旅人養成組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.05_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4938662395\",\"Name\":\"落雪的贈禮\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.03\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"4938772553\",\"Name\":\"雪願閃耀·超值閃碟組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_s\",\"GameProductID\":\"pack.05_disc_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5027109460\",\"Name\":\"平安度過試用期\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.04\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5027264811\",\"Name\":\"碧玉璀璨·豪華閃碟組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_l\",\"GameProductID\":\"pack.06_disc_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5027298656\",\"Name\":\"碧玉璀璨·傾心禮品組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.06_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5027413778\",\"Name\":\"碧玉璀璨·旅人養成組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.06_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5027550435\",\"Name\":\"碧玉璀璨·特惠禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.06_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5027674780\",\"Name\":\"碧玉璀璨·豪華禮券組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_l\",\"GameProductID\":\"pack.06_role_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5027803159\",\"Name\":\"碧玉璀璨·超值禮券組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_s\",\"GameProductID\":\"pack.06_role_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5027853776\",\"Name\":\"碧玉璀璨·超值閃碟組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_s\",\"GameProductID\":\"pack.06_disc_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5027907414\",\"Name\":\"碧玉璀璨·特惠閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.06_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029044394\",\"Name\":\"錦繡迎春·旅人養成組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.07_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029048261\",\"Name\":\"錦繡迎春·特惠閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.07_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029262194\",\"Name\":\"錦繡迎春·攀星禮券組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_sd\",\"GameProductID\":\"pack.07_role_sd\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029322097\",\"Name\":\"錦繡迎春·超值禮券組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_s\",\"GameProductID\":\"pack.07_role_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029457334\",\"Name\":\"錦繡迎春·豪華禮券組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_l\",\"GameProductID\":\"pack.07_role_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029479965\",\"Name\":\"盈月星愿.攀登\",\"Price\":60,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.monthlycard.strength\",\"GameProductID\":\"monthlyCard.strength\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029618924\",\"Name\":\"錦繡迎春·豪華閃碟組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_l\",\"GameProductID\":\"pack.07_disc_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029766504\",\"Name\":\"可可甜心情與夜\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.05\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029801170\",\"Name\":\"錦繡迎春·特惠禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.07_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029885955\",\"Name\":\"錦繡迎春·鴻運小禮\",\"Price\":33,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.hongbao\",\"GameProductID\":\"pack.07_hongbao\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029893211\",\"Name\":\"錦繡迎春·技能提升組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.skill_xc\",\"GameProductID\":\"pack.07_skill_xc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029897788\",\"Name\":\"錦繡迎春·超值閃碟組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_s\",\"GameProductID\":\"pack.07_disc_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5029954326\",\"Name\":\"錦繡迎春·攀星閃碟組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_sd\",\"GameProductID\":\"pack.07_disc_sd\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424063372\",\"Name\":\"寒鋒素裹·超值禮券組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_s\",\"GameProductID\":\"pack.08_role_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424100247\",\"Name\":\"寒鋒素裹·特惠禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.08_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424133299\",\"Name\":\"寒鋒素裹·旅人養成組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.08_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424149993\",\"Name\":\"寒鋒素裹·豪華閃碟組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_l\",\"GameProductID\":\"pack.08_disc_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424153721\",\"Name\":\"使命必達·豪華禮券組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_l\",\"GameProductID\":\"pack.09_role_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424183027\",\"Name\":\"使命必達·豪華閃碟組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_l\",\"GameProductID\":\"pack.09_disc_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424219235\",\"Name\":\"雲海行歌·特惠閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc1\",\"GameProductID\":\"pack.09_1_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424259100\",\"Name\":\"寒鋒素裹·特惠閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.08_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424276365\",\"Name\":\"雲海行歌·特惠禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role_1\",\"GameProductID\":\"pack.09_1_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424279263\",\"Name\":\"寒鋒素裹·傾心禮品組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.08_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424442198\",\"Name\":\"使命必達·超值閃碟組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_s\",\"GameProductID\":\"pack.09_disc_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424473442\",\"Name\":\"使命必達·特惠閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.09_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424534621\",\"Name\":\"使命必達·傾心禮品組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.09_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424649833\",\"Name\":\"寒鋒素裹·超值閃碟組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_s\",\"GameProductID\":\"pack.08_disc_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424661578\",\"Name\":\"使命必達·特惠禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.09_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424717323\",\"Name\":\"清露微滴的初夏\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.07\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424809875\",\"Name\":\"相約融雪後\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.06\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424865611\",\"Name\":\"使命必達·旅人養成組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.09_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424869844\",\"Name\":\"寒鋒素裹·豪華禮券組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_l\",\"GameProductID\":\"pack.08_role_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5424982092\",\"Name\":\"使命必達·超值禮券組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_s\",\"GameProductID\":\"pack.09_role_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765116538\",\"Name\":\"落月芬香·豪華禮券組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_l\",\"GameProductID\":\"pack.10_role_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765206015\",\"Name\":\"曉露朝顏，少女初覺\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.08\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765365689\",\"Name\":\"落月芬香·豪華閃碟組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_l\",\"GameProductID\":\"pack.10_disc_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765386582\",\"Name\":\"落月芬香·旅人養成組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.10_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765415335\",\"Name\":\"古靈精怪茶話會\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin1.98\",\"GameProductID\":\"skin.98.09\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765482837\",\"Name\":\"落月芬香·超值閃碟組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_s\",\"GameProductID\":\"pack.10_disc_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765521622\",\"Name\":\"落月芬香·特惠禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.10_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765553061\",\"Name\":\"沐星邀約·自選祕紋組合\",\"Price\":630,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc.sel\",\"GameProductID\":\"pack.10_disc_sel\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765560161\",\"Name\":\"落月芬香·傾心禮品組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.10_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765750199\",\"Name\":\"沐星邀約·自選旅人組合\",\"Price\":630,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role.sel\",\"GameProductID\":\"pack.10_role_sel\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765812048\",\"Name\":\"落月芬香·超值禮券組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_s\",\"GameProductID\":\"pack.10_role_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5765876618\",\"Name\":\"落月芬香·特惠閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.10_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766097830\",\"Name\":\"萬象初見·超值禮券組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_s\",\"GameProductID\":\"pack.11_role_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766135140\",\"Name\":\"奇巧匠箱·特惠閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.12_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766172099\",\"Name\":\"萬象初見·豪華閃碟組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_l\",\"GameProductID\":\"pack.11_disc_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766266987\",\"Name\":\"奇巧匠箱·特惠禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.12_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766315725\",\"Name\":\"歡騰巨浪·傾心禮品組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.12_1_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766460038\",\"Name\":\"萬象初見·豪華禮券組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_l\",\"GameProductID\":\"pack.11_role_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766498650\",\"Name\":\"奇巧匠箱·傾心禮品組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.12_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766518226\",\"Name\":\"奇巧匠箱·旅人養成組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.12_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766531189\",\"Name\":\"琉璃倒影☆寂寥呢喃\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.10\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766616099\",\"Name\":\"歡騰巨浪·超值禮券組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_s\",\"GameProductID\":\"pack.12_1_role_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766626354\",\"Name\":\"歡騰巨浪·特惠禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.12_1_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766733577\",\"Name\":\"奇巧匠箱·豪華禮券組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_l\",\"GameProductID\":\"pack.12_role_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766751867\",\"Name\":\"歡騰巨浪·特惠閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.12_1_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766762760\",\"Name\":\"萬象初見·特惠閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.11_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766787026\",\"Name\":\"奇巧匠箱·超值閃碟組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_s\",\"GameProductID\":\"pack.12_disc_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766791502\",\"Name\":\"歡騰巨浪·旅人養成組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.12_1_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766809745\",\"Name\":\"歡騰巨浪·豪華禮券組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_l\",\"GameProductID\":\"pack.12_1_role_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766811980\",\"Name\":\"萬象初見·傾心禮品組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.11_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766814648\",\"Name\":\"靜享沁涼 悠然一夏\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.11\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766823244\",\"Name\":\"奇巧匠箱·超值禮券組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_s\",\"GameProductID\":\"pack.12_role_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766851946\",\"Name\":\"歡騰巨浪·超值閃碟組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_s\",\"GameProductID\":\"pack.12_1_disc_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766901621\",\"Name\":\"歡騰巨浪·豪華閃碟組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_l\",\"GameProductID\":\"pack.12_1_disc_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766909970\",\"Name\":\"萬象初見·超值閃碟組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_s\",\"GameProductID\":\"pack.11_disc_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766954338\",\"Name\":\"萬象初見·特惠禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.11_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766969134\",\"Name\":\"萬象初見·旅人養成組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.11_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"5766995351\",\"Name\":\"奇巧匠箱·豪華閃碟組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_l\",\"GameProductID\":\"pack.12_disc_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"6222387395\",\"Name\":\"日曬的餘溫\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.01.1\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"6222554446\",\"Name\":\"異國風起時\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.02.1\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"6290021207\",\"Name\":\"鏡中的紅心皇后\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.skin.98\",\"GameProductID\":\"skin.98.12\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"6290048062\",\"Name\":\"暗黑魔典·豪華閃碟組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_l\",\"GameProductID\":\"pack.13_disc_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"6290091163\",\"Name\":\"暗黑魔典·旅人養成組合\",\"Price\":400,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.res\",\"GameProductID\":\"pack.13_res\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"6290243194\",\"Name\":\"暗黑魔典·傾心禮品組合\",\"Price\":290,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.gift\",\"GameProductID\":\"pack.13_gift\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"6290385618\",\"Name\":\"暗黑魔典·豪華禮券組合\",\"Price\":1100,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_l\",\"GameProductID\":\"pack.13_role_l\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"6290435870\",\"Name\":\"暗黑魔典·超值禮券組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.role_s\",\"GameProductID\":\"pack.13_role_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"6290751133\",\"Name\":\"暗黑魔典·特惠閃碟組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_disc\",\"GameProductID\":\"pack.13_disc\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"6290771520\",\"Name\":\"暗黑魔典·超值閃碟組合\",\"Price\":750,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.disc_s\",\"GameProductID\":\"pack.13_disc_s\",\"CurrencyCode\":\"TWD\",\"ProductType\":1},{\"ID\":\"6290844832\",\"Name\":\"暗黑魔典·特惠禮券組合\",\"Price\":490,\"Desc\":\"\",\"StoreProductID\":\"com.stargazer.stellasora.pack.new_role\",\"GameProductID\":\"pack.13_role\",\"CurrencyCode\":\"TWD\",\"ProductType\":1}]},\"Msg\":\"OK\"}");

    struct HttpOrderProduct
    {
        std::string ProductId;
        std::string StoreProductId;
        std::string GameProductId;
        int Price = 0;
        std::string Name;
    };

    const std::unordered_map<std::string, HttpOrderProduct>& GetHttpOrderProducts()
    {
        static const std::unordered_map<std::string, HttpOrderProduct> kProducts = []() {
            std::unordered_map<std::string, HttpOrderProduct> out;
            if (kOrderProductsResponse.empty())
            {
                return out;
            }

            try
            {
                const auto root = nlohmann::json::parse(kOrderProductsResponse);
                const auto& list = root.at("Data").at("List");
                for (const auto& item : list)
                {
                    HttpOrderProduct product;
                    product.ProductId = item.value("ID", "");
                    product.StoreProductId = item.value("StoreProductID", "");
                    product.GameProductId = item.value("GameProductID", "");
                    product.Price = item.value("Price", 0);
                    product.Name = item.value("Name", "");
                    if (!product.ProductId.empty())
                    {
                        out.emplace(product.ProductId, std::move(product));
                    }
                }
            }
            catch (const std::exception& e)
            {
                LOG_WARNING("解析商品列表失败: {}", e.what());
            }

            return out;
            }();

        return kProducts;
    }

}

PaymentContextService& PaymentContextService::Instance()
{
    static PaymentContextService instance;
    return instance;
}

void PaymentContextService::RegisterWebOrderContext(const WebOrderContext& context)
{
    if (context.Token.empty())
    {
        return;
    }

    std::lock_guard lock(mWebOrderMutex);
    mWebOrderContexts[context.Token] = context;
}

bool PaymentContextService::GetWebOrderContext(const std::string& token, WebOrderContext& outContext)
{
    if (token.empty())
    {
        return false;
    }

    std::lock_guard lock(mWebOrderMutex);
    const auto it = mWebOrderContexts.find(token);
    if (it == mWebOrderContexts.end())
    {
        return false;
    }

    outContext = it->second;
    return true;
}

void PaymentContextService::RemoveWebOrderContext(const std::string& token)
{
    if (token.empty())
    {
        return;
    }

    std::lock_guard lock(mWebOrderMutex);
    mWebOrderContexts.erase(token);
}

void OrderProductsHandler(const HttpRequest&, HttpResponse& rsp)
{
    rsp.statusCode = 200;
    rsp.headers["Content-Type"] = "application/json; charset=utf-8";
    rsp.body = kOrderProductsResponse;
}

void OrderCreateHandler(const HttpRequest& req, HttpResponse& rsp)
{
    nlohmann::json reqJson;
    try
    {
        reqJson = nlohmann::json::parse(req.body);
    }
    catch (const std::exception& e)
    {
        LOG_WARNING("订单创建请求解析失败: {}", e.what());
        nlohmann::json errorResp;
        errorResp["Code"] = ResultCode::CLIENT_PARAMETER_ERROR;
        errorResp["Data"] = nlohmann::json::object();
        errorResp["Msg"] = U8("请求无效");
        rsp.statusCode = 200;
        rsp.headers["Content-Type"] = "application/json; charset=utf-8";
        rsp.body = errorResp.dump();
        return;
    }

    const std::string productId = reqJson.value("ProductId", "");
    const std::string extraData = reqJson.value("ExtraData", "");
    const auto& products = GetHttpOrderProducts();
    const auto productIt = products.find(productId);
    if (!products.empty() && productIt == products.end())
    {
        nlohmann::json errorResp;
        errorResp["Code"] = ResultCode::PAY_PRODUCTID_NOT_EXIST;
        errorResp["Data"] = nlohmann::json::object();
        errorResp["Msg"] = U8("商品不存在");
        rsp.statusCode = 200;
        rsp.headers["Content-Type"] = "application/json; charset=utf-8";
        rsp.body = errorResp.dump();
        return;
    }

    PaymentContextService::WebOrderContext context;
    const bool hasContext = PaymentContextService::Instance().GetWebOrderContext(extraData, context);

    std::string orderId;
    if (!GenerateToken(orderId, false))
    {
        nlohmann::json errorResp;
        errorResp["Code"] = ResultCode::SERVER_ERROR;
        errorResp["Data"] = nlohmann::json::object();
        errorResp["Msg"] = U8("订单创建失败");
        rsp.statusCode = 200;
        rsp.headers["Content-Type"] = "application/json; charset=utf-8";
        rsp.body = errorResp.dump();
        return;
    }

    const int64_t createdAt = GameTime::NowSeconds();
    const std::string redirectUrl = "http://" + Config::Get().httpServerConfig.publicIp + ":" + std::to_string(Config::Get().httpServerConfig.port) + "/mock-pay?orderId=" + orderId + "&productId=" + productId;;
    const std::string storeProductId = productIt != products.end() ? productIt->second.StoreProductId : std::string();

    nlohmann::json order;
    order["CreatedAt"] = createdAt;
    order["GameExtraData"] = extraData;
    order["ID"] = orderId;
    order["StoreName"] = "mock";
    order["StoreProductID"] = storeProductId;

    if (hasContext)
    {
        order["LinkedGameOrderID"] = context.GameOrderId;
        order["LinkedSource"] = context.Source;
        order["LinkedProductKey"] = context.ProductKey;
        order["LinkedPlayerUID"] = context.PlayerUid;
    }

    nlohmann::json resp;
    resp["Code"] = 200;
    resp["Data"]["Order"] = order;
    resp["Data"]["PC"]["RedirectURL"] = redirectUrl;
    resp["Msg"] = "OK";

    rsp.statusCode = 200;
    rsp.headers["Content-Type"] = "application/json; charset=utf-8";
    rsp.body = resp.dump();
}

void MockPayPageHandler(const HttpRequest& req, HttpResponse& rsp)
{
    const std::string orderId = req.GetQueryParam("orderId");
    const std::string productId = req.GetQueryParam("productId");

    std::string html = U8(
        "<!doctype html><html><head><meta charset=\"utf-8\">"
        "<title>Mock Pay</title>"
        "<style>"
        "body{font-family:Segoe UI,Microsoft YaHei,sans-serif;background:#f6f3ed;color:#222;margin:0;}"
        ".wrap{max-width:720px;margin:64px auto;padding:32px;background:#fff;border:1px solid #ddd;border-radius:16px;box-shadow:0 8px 30px rgba(0,0,0,.06);}"
        "h1{margin-top:0;font-size:44px;}"
        "p{font-size:24px;line-height:1.6;}"
        "code{background:#f3f3f3;padding:2px 6px;border-radius:6px;font-size:22px;}"
        ".btn{display:inline-block;margin-top:20px;padding:12px 18px;background:#1f6feb;color:#fff;text-decoration:none;border-radius:10px;}"
        ".muted{color:#666;}"
        ".close-hint{font-size:20px;color:#888;margin-top:20px;}"
        "</style></head><body><div class=\"wrap\">"
        "<h1>Mock Pay</h1>"
        "<p>订单号：<code>") + orderId + U8("</code></p>"
            "<p>商品 ID：<code>") + productId + U8("</code></p>"
                "<p class=\"close-hint\">请点击左上角按钮关闭</p>"
                "</div></body></html>");

    rsp.statusCode = 200;
    rsp.headers["Content-Type"] = "text/html; charset=utf-8";
    rsp.body = html;
}

void OrderNotifyHandler(const HttpRequest&, HttpResponse& rsp)
{
    nlohmann::json resp;
    resp["Code"] = 200;
    resp["Data"] = nlohmann::json::object();
    resp["Msg"] = "OK";

    rsp.statusCode = 200;
    rsp.headers["Content-Type"] = "application/json; charset=utf-8";
    rsp.body = resp.dump();
}
