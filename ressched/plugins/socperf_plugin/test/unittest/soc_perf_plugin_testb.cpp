/*
 * @tc.name: SocPerfPluginTest_API_TEST_057
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require:
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_057, Function | MediumTest | Level0)
{
    bool ret = SocPerfPlugin::GetInstance().HandleRssCloudConfigUpdate(nullptr);
    EXPECT_EQ(ret, false);
    nlohmann::json payload = nlohmann::json::parse(R"({
	"params" : {
        "SOCPERF": {
            "specialExtension": {
                "itemList": [
                    {
                        "itemProperties": {
                            "key": "specialExtension"
                        },
                        "subItemList": [
                            {
                                "name": "info",
                                "value": "0",
                                "properties": {
                                    "bundleName" : "1",
									"callerBundleName" : ""
                                }
                            }
                        ]
                    }
                ]
            }
        }
    }
    })");
    const std::shared_ptr<ResData>& data = std::make_shared<ResData>(ResType::RES_TYPE_RSS_CLOUD_CONFIG_UPDATE,
        0, payload);
    ret = SocPerfPlugin::GetInstance().HandleRssCloudConfigUpdate(data);
    EXPECT_EQ(ret, true);
}
 
/*
 * @tc.name: SocPerfPluginTest_API_TEST_058
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require:
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_058, Function | MediumTest | Level0)
{
    bool ret = SocPerfPlugin::GetInstance().HandleRssCloudConfigUpdate(nullptr);
    EXPECT_EQ(ret, false);
    nlohmann::json payload = nlohmann::json::parse(R"({
	"params" : {
        "SOCPERF": {
            "weakInterAction": {
                "itemList": [
                    {
                        "itemProperties": {
                            "key": "enable"
                        },
                        "subItemList": [
                            {
                                "name": "enable",
                                "value": "1",
                                "properties": {}
                            }
                        ]
                    }
                ]
            }
        }
    }
    })");
    const std::shared_ptr<ResData>& data = std::make_shared<ResData>(ResType::RES_TYPE_RSS_CLOUD_CONFIG_UPDATE,
        0, payload);
    ret = SocPerfPlugin::GetInstance().HandleRssCloudConfigUpdate(data);
    EXPECT_EQ(ret, true);
}
 
/*
 * @tc.name: SocPerfPluginTest_API_TEST_059
 * @tc.desc: test socperfplugin api
 * @tc.type FUNC
 * @tc.require:
 */
HWTEST_F(SocPerfPluginTest, SocPerfPluginTest_API_TEST_059, Function | MediumTest | Level0)
{
    bool ret = SocPerfPlugin::GetInstance().HandleRssCloudConfigUpdate(nullptr);
    EXPECT_EQ(ret, false);
    nlohmann::json payload = nlohmann::json::parse(R"({
	"params" : {
        "SOCPERF": {
            "specialExtension": {
                "itemList": [
                    {
                        "itemProperties": {
                            "key": "specialExtension"
                        },
                        "subItemList": [
                            {
                                "name": "info",
                                "value": "0",
                                "properties": {
                                    "bundleName" : "2",
									"callerBundleName" : "1"
                                }
                            }
                        ]
                    }
                ]
            },
            "weakInterAction": {
                "itemList": [
                    {
                        "itemProperties": {
                            "key": "enable"
                        },
                        "subItemList": [
                            {
                                "name": "enable",
                                "value": "0",
                                "properties": {}
                            }
                        ]
                    }
                ]
            }
        }
    }
    })");
    const std::shared_ptr<ResData>& data = std::make_shared<ResData>(ResType::RES_TYPE_RSS_CLOUD_CONFIG_UPDATE,
        0, payload);
    ret = SocPerfPlugin::GetInstance().HandleRssCloudConfigUpdate(data);
    EXPECT_EQ(ret, true);
}

bool SocPerfPlugin::HandleRssCloudConfigUpdate(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr ||
        data->payload.is_null() ||
        !data->payload.contains(CLOUD_PARAMS) ||
        !data->payload[CLOUD_PARAMS].is_object()) {
        return false;
    }
 
    if (!data->payload[CLOUD_PARAMS].contains(PLUGIN_NAME) ||
        !data->payload[CLOUD_PARAMS].at(PLUGIN_NAME).is_object()) {
        return false;
    }
 
    PluginConfigMap pluginConfigs = (data->payload)[CLOUD_PARAMS][PLUGIN_NAME].get<PluginConfigMap>();
    if (pluginConfigs.find(SPECIAL_EXTENSION_STRING) != pluginConfigs.end()) {
        LoadSpecialExtension(pluginConfigs[SPECIAL_EXTENSION_STRING]);
    }
    if (pluginConfigs.find(WEAK_ACTION_STRING) != pluginConfigs.end()) {
        LoadWeakInterAction(pluginConfigs[WEAK_ACTION_STRING]);
    }
    return true;
}
 
bool SocPerfPlugin::ReportAbilityStatus(const std::shared_ptr<ResData>& data)
{
    if (data == nullptr || data->payload == nullptr) {
        return false;
    }
 
    if (!data->payload.contains("saId") || !data->payload.at("saId").is_number_integer()) {
        return false;
    }
 
    int32_t saId = data->payload["saId"].get<int32_t>();
    if (saId == 1906 && data->value > 0) {
        SOC_PERF_LOGI("SocPerfPlugin: socperf start");
        UpdateWeakActionStatus();
        return true;
    }
    return false;
}