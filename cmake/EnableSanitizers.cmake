#          Copyright Dominic (DNKpp) Koepke 2025 - 2025.
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)

function(enable_sanitizers TARGET_NAME)
	find_package(sanitizers-cmake)

	if (SANITIZE_ADDRESS)
		# workaround linker errors on msvc
		# see: https://learn.microsoft.com/en-us/answers/questions/864574/enabling-address-sanitizer-results-in-error-lnk203
		target_compile_definitions(${TARGET_NAME}
			PRIVATE
			$<$<CXX_COMPILER_ID:MSVC>:_DISABLE_VECTOR_ANNOTATION>
			$<$<CXX_COMPILER_ID:MSVC>:_DISABLE_STRING_ANNOTATION>
		)
	endif()

	add_sanitizers(${TARGET_NAME})
endfunction()