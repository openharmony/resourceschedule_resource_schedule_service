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