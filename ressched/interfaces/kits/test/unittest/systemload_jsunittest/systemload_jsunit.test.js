/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import systemload from '@ohos.resourceschedule.systemload'

import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from 'deccjsunit/index'

describe("SystemloadJsTest", function () {
    function onSystemloadChange1(systemloadLevel) {
        console.info('systemload level 1 : ' + systemloadLevel)
    }

    function onSystemloadChange2(systemloadLevel) {
        console.info('systemload level 2 : ' + systemloadLevel)
    }

    beforeAll(function() {
        /*
         * @tc.setup: setup invoked before all testcases
         */
        console.info('beforeAll called')
    })

    afterAll(function() {
        /*
         * @tc.teardown: teardown invoked after all testcases
         */
        console.info('afterAll called')
    })

    beforeEach(function() {
        /*
         * @tc.setup: setup invoked before each testcases
         */
        console.info('beforeEach called')
    })

    afterEach(function() {
        /*
         * @tc.teardown: teardown invoked after each testcases
         */
        systemload.off("systemLoadChange", onSystemloadChange1);
        systemload.off("test", onSystemloadChange1);
        console.info('afterEach called')
    })

    /*
     * @tc.name: SystemloadJsTest001
     * @tc.desc: test register systemload callback
     * @tc.type: FUNC
     * @tc.require: issueI97M6C
     */
    it("SystemloadJsTest001", 0, async function (done) {
        console.info('----------------------SystemloadJsTest001---------------------------');
        try {
            systemload.on("systemLoadChange", onSystemloadChange1);
        } catch (error) {
            expect(true).assertEqual(true);
        }
        done();
    })

    /*
     * @tc.name: SystemloadJsTest002
     * @tc.desc: test register systemload callback
     * @tc.type: FUNC
     * @tc.require: issueI97M6C
     */
    it("SystemloadJsTest002", 0, async function (done) {
        console.info('----------------------SystemloadJsTest002---------------------------');
        try {
            systemload.on("systemLoadChange", onSystemloadChange1);
            systemload.on("systemLoadChange", onSystemloadChange2);
        } catch (error) {
            expect(true).assertEqual(true);
        }
        done();
    })

    /*
     * @tc.name: SystemloadJsTest003
     * @tc.desc: test register systemload callback
     * @tc.type: FUNC
     * @tc.require: issueI97M6C
     */
    it("SystemloadJsTest003", 0, async function (done) {
        console.info('----------------------SystemloadJsTest003---------------------------');
        try {
            systemload.on("systemLoadChange", onSystemloadChange1);
            systemload.on("test", onSystemloadChange2);
        } catch (error) {
            expect(true).assertEqual(true);
        }
        done();
    })

    /*
     * @tc.name: SystemloadJsTest004
     * @tc.desc: test unRegister systemload callback
     * @tc.type: FUNC
     * @tc.require: issueI97M6C
     */
    it("SystemloadJsTest004", 0, async function (done) {
        console.info('----------------------SystemloadJsTest004---------------------------');
        try {
            systemload.on("systemLoadChange", onSystemloadChange1);
            systemload.off("systemLoadChange", onSystemloadChange1);
        } catch (error) {
            expect(true).assertEqual(true);
        }
        done();
    })

    /*
     * @tc.name: SystemloadJsTest005
     * @tc.desc: test unRegister systemload callback
     * @tc.type: FUNC
     * @tc.require: issueI97M6C
     */
    it("SystemloadJsTest005", 0, async function (done) {
        console.info('----------------------SystemloadJsTest005---------------------------');
        try {
            systemload.on("systemLoadChange", onSystemloadChange1);
            systemload.off("test", onSystemloadChange1);
        } catch (error) {
            expect(true).assertEqual(true);
        }
        done();
    })

    /*
     * @tc.name: SystemloadJsTest006
     * @tc.desc: test unRegister systemload callback
     * @tc.type: FUNC
     * @tc.require: issueI97M6C
     */
    it("SystemloadJsTest006", 0, async function (done) {
        console.info('----------------------SystemloadJsTest006---------------------------');
        try {
            systemload.off("systemLoadChange", onSystemloadChange1);
        } catch (error) {
            expect(true).assertEqual(true);
        }
        done();
    })

    /*
     * @tc.name: SystemloadJsTest007
     * @tc.desc: test get systemload level
     * @tc.type: FUNC
     * @tc.require: issueI97M6C
     */
    it("SystemloadJsTest007", 0, async function (done) {
        console.info('----------------------SystemloadJsTest007---------------------------');
        try {
            systemload.getLevel().then((res) => {
                console.log("systemload level : " + res);
            }).catch((err) => {
                expect(false).assertEqual(true);
            });
        } catch (error) {
            expect(false).assertEqual(true);
        }
        done();
    })
})
