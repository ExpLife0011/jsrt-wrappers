// Copyright 2013 Paul Vick
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stdafx.h"
#include "jsrt_wrappers.h"

namespace jsrt
{
    void runtime::translate_error_code(JsErrorCode errorCode)
    {
        switch (errorCode)
        {
        case JsNoError:
            return;
        case JsErrorInvalidArgument:
            throw invalid_argument_exception();
        case JsErrorNullArgument:
            throw null_argument_exception();
        case JsErrorNoCurrentContext:
            throw no_current_context_exception();
        case JsErrorInExceptionState:
            throw in_exception_state_exception();
        case JsErrorNotImplemented:
            throw not_implemented_exception();
        case JsErrorWrongThread:
            throw wrong_thread_exception();
        case JsErrorRuntimeInUse:
            throw runtime_in_use_exception();
        case JsErrorBadSerializedScript:
            throw bad_serialized_script_exception();
        case JsErrorInDisabledState:
            throw in_disabled_state_exception();
        case JsErrorCannotDisableExecution:
            throw cannot_disable_execution_exception();
        case JsErrorHeapEnumInProgress:
            throw heap_enum_in_progress_exception();
        case JsErrorArgumentNotObject:
            throw argument_not_object_exception();
        case JsErrorInProfileCallback:
            throw in_profile_callback_exception();
        case JsErrorInThreadServiceCallback:
            throw in_thread_service_callback_exception();
        case JsErrorCannotSerializeDebugScript:
            throw cannot_serialize_debug_script_exception();
        case JsErrorAlreadyDebuggingContext:
            throw already_debugging_context_exception();
        case JsErrorAlreadyProfilingContext:
            throw already_profiling_context_exception();
        case JsErrorIdleNotEnabled:
            throw idle_not_enabled_exception();
        case JsErrorOutOfMemory:
            throw out_of_memory_exception();
        case JsErrorScriptException:
            {
                JsValueRef exception;

                if (JsGetAndClearException(&exception) != JsNoError)
                {
                    // Something has gone very wrong.
                    throw new fatal_exception();
                }

                throw script_exception(value(exception));
            }
        case JsErrorScriptCompile:
            {
                JsValueRef exception;

                if (JsGetAndClearException(&exception) != JsNoError)
                {
                    // Something has gone very wrong.
                    throw new fatal_exception();
                }

                throw script_compile_exception(compile_error(exception));
            }
        case JsErrorScriptTerminated:
            throw script_terminated_exception();
        case JsErrorScriptEvalDisabled:
            throw script_eval_disabled_exception();
        case JsErrorFatal:
            throw fatal_exception();
        default:
            throw fatal_exception();
        }
    }

    context runtime::create_context(IDebugApplication *site)
    {
        JsContextRef newContext;

        runtime::translate_error_code(JsCreateContext(_handle, site, &newContext));

        return context(newContext);
    }

    void context::set_exception(value object)
    {
        runtime::translate_error_code(JsSetException(object.handle()));
    }

    value context::get_and_clear_exception()
    {
        JsValueRef exception;
        runtime::translate_error_code(JsGetAndClearException(&exception));
        return value(exception);
    }

    function_base context::parse(std::wstring script, JsSourceContext sourceContext, std::wstring sourceUrl)
    {
        JsValueRef result = nullptr;

        runtime::translate_error_code(JsParseScript(script.c_str(), sourceContext, sourceUrl.c_str(), &result));

        return function_base(result);
    }

    value context::evaluate(std::wstring script, JsSourceContext sourceContext, std::wstring sourceUrl)
    {
        JsValueRef result = nullptr;

        runtime::translate_error_code(JsRunScript(script.c_str(), sourceContext, sourceUrl.c_str(), &result));

        return value(result);
    }

    function_base context::parse_serialized(std::wstring script, unsigned char *buffer, JsSourceContext sourceContext, std::wstring sourceUrl)
    {
        JsValueRef result = nullptr;

        runtime::translate_error_code(JsParseSerializedScript(script.c_str(), buffer, sourceContext, sourceUrl.c_str(), &result));

        return function_base(result);
    }

    value context::evaluate_serialized(std::wstring script, unsigned char *buffer, JsSourceContext sourceContext, std::wstring sourceUrl)
    {
        JsValueRef result = nullptr;

        runtime::translate_error_code(JsRunSerializedScript(script.c_str(), buffer, sourceContext, sourceUrl.c_str(), &result));

        return value(result);
    }

    boolean value::to_boolean()
    {
        JsValueRef booleanValue;
        runtime::translate_error_code(JsConvertValueToBoolean(handle(), &booleanValue));
        return boolean(booleanValue);
    }

    number value::to_number()
    {
        JsValueRef numberValue;
        runtime::translate_error_code(JsConvertValueToNumber(handle(), &numberValue));
        return number(numberValue);
    }

    string value::to_string()
    {
        JsValueRef stringValue;
        runtime::translate_error_code(JsConvertValueToString(handle(), &stringValue));
        return string(stringValue);
    }

    template<class T>
    property_descriptor<T> object::get_own_property_descriptor(property_id name)
    {
        JsValueRef valueHandle;
        runtime::translate_error_code(JsGetOwnPropertyDescriptor(handle(), name.handle(), &valueHandle));

        return property_descriptor<T>(value(valueHandle));
    }

    template<class T>
    bool object::define_property(property_id name, property_descriptor<T> descriptor)
    {
        bool value;
        runtime::translate_error_code(JsDefineProperty(handle(), name.handle(), descriptor.handle(), &value));

        return value;
    }

    std::vector<std::wstring> object::get_own_property_names()
    {
        std::vector<std::wstring> namesVector;

        JsValueRef names;
        runtime::translate_error_code(JsGetOwnPropertyNames(handle(), &names));

        auto namesArray = array<std::wstring>(names);

        for (int index = 0; index < namesArray.length(); index++)
        {
            namesVector.push_back(namesArray[index]);
        }

        return namesVector;
    }
}