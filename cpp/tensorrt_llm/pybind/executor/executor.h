/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once
#include "tensorrt_llm/executor/executor.h"
#include "tensorrt_llm/executor/types.h"
#include <pybind11/pybind11.h>

namespace tle = tensorrt_llm::executor;

namespace tensorrt_llm::pybind::executor
{

class Executor
{
public:
    Executor(
        std::filesystem::path const& modelPath, tle::ModelType modelType, tle::ExecutorConfig const& executorConfig);

    Executor(std::string const& engineBuffer, std::string const& jsonConfigStr, tle::ModelType modelType,
        tle::ExecutorConfig const& executorConfig);

    pybind11::object enter();
    void exit([[maybe_unused]] pybind11::handle type, [[maybe_unused]] pybind11::handle value,
        [[maybe_unused]] pybind11::handle traceback);
    void shutdown();

    [[nodiscard]] tle::IdType enqueueRequest(tle::Request request)
    {
        return mExecutor->enqueueRequest(std::move(request));
    }

    [[nodiscard]] std::vector<tle::IdType> enqueueRequests(std::vector<tle::Request> requests)
    {
        return mExecutor->enqueueRequests(std::move(requests));
    }

    [[nodiscard]] std::vector<tle::Response> awaitResponses(
        std::optional<std::chrono::milliseconds> const& timeout = std::nullopt)
    {
        // Await responses blocks until a response is received. Release GIL so that it can be ran in a background
        // thread.
        pybind11::gil_scoped_release release;
        return mExecutor->awaitResponses(timeout);
    }

    [[nodiscard]] std::vector<tle::Response> awaitResponses(
        tle::IdType const& requestId, std::optional<std::chrono::milliseconds> const& timeout = std::nullopt)
    {
        // Await responses blocks until a response is received. Release GIL so that it can be ran in a background
        // thread.
        pybind11::gil_scoped_release release;
        return mExecutor->awaitResponses(requestId, timeout);
    }

    [[nodiscard]] std::vector<std::vector<tle::Response>> awaitResponses(std::vector<tle::IdType> const& requestIds,
        std::optional<std::chrono::milliseconds> const& timeout = std::nullopt)
    {
        // Await responses blocks until a response is received. Release GIL so that it can be ran in a background
        // thread.
        pybind11::gil_scoped_release release;
        return mExecutor->awaitResponses(requestIds, timeout);
    }

    [[nodiscard]] tle::SizeType getNumResponsesReady(std::optional<tle::IdType> const& requestId = std::nullopt) const
    {
        return mExecutor->getNumResponsesReady(requestId);
    }

    void cancelRequest(tle::IdType requestId)
    {
        mExecutor->cancelRequest(requestId);
    }

    std::deque<tle::IterationStats> getLatestIterationStats()
    {
        return mExecutor->getLatestIterationStats();
    }

    std::deque<tle::RequestStatsPerIteration> getLatestRequestStats()
    {
        return mExecutor->getLatestRequestStats();
    }

    [[nodiscard]] bool canEnqueueRequests() const
    {
        return mExecutor->canEnqueueRequests();
    }

    static void initBindings(pybind11::module_& m);

private:
    std::unique_ptr<tle::Executor> mExecutor;
};

} // namespace tensorrt_llm::pybind::executor
