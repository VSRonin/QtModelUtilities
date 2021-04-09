/****************************************************************************\
   Copyright 2018 Luca Beldi
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at
       http://www.apache.org/licenses/LICENSE-2.0
   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
\****************************************************************************/
#ifndef MODELUTILITIES_GLOBAL_H
#define MODELUTILITIES_GLOBAL_H
#include <QtGlobal>
#ifndef MODELUTILITIES_STATIC
#    if defined(MODELUTILITIES_LIB)
#        define MODELUTILITIES_EXPORT Q_DECL_EXPORT
#    else
#        define MODELUTILITIES_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define MODELUTILITIES_EXPORT
#endif
#endif // MODELUTILITIES_GLOBAL_H
