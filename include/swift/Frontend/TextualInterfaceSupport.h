//===--- TextualInterfaceSupport.h - swiftinterface files ----*- C++ -*-===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2018 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See https://swift.org/LICENSE.txt for license information
// See https://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#ifndef SWIFT_FRONTEND_TEXTUALINTERFACESUPPORT_H
#define SWIFT_FRONTEND_TEXTUALINTERFACESUPPORT_H

#include "swift/Basic/LLVM.h"
#include "swift/Serialization/SerializedModuleLoader.h"
#include "llvm/Support/Regex.h"

namespace swift {

class ModuleDecl;

/// Options for controlling the generation of the .swiftinterface output.
struct TextualInterfaceOptions {
  /// Copy of all the command-line flags passed at .swiftinterface
  /// generation time, re-applied to CompilerInvocation when reading
  /// back .swiftinterface and reconstructing .swiftmodule.
  std::string TextualInterfaceFlags;
};

llvm::Regex getSwiftInterfaceToolsVersionRegex();
llvm::Regex getSwiftInterfaceModuleFlagsRegex();

/// Emit a stable, textual interface for \p M, which can be used by a client
/// source file to import this module, subject to options given by \p Opts.
///
/// Unlike a serialized module, the textual format generated by
/// emitModuleInterface is intended to be stable across compiler versions while
/// still describing the full ABI of the module in question.
///
/// The initial plan for this format can be found at
/// https://forums.swift.org/t/plan-for-module-stability/14551/
///
/// \return true if an error occurred
///
/// \sa swift::serialize
bool emitModuleInterface(raw_ostream &out,
                         TextualInterfaceOptions const &Opts,
                         ModuleDecl *M);


/// A ModuleLoader that runs a subordinate \c CompilerInvocation and \c
/// CompilerInstance to convert .swiftinterface files to .swiftmodule
/// files on the fly, caching the resulting .swiftmodules in the module cache
/// directory, and loading the serialized .swiftmodules from there.
class TextualInterfaceModuleLoader : public SerializedModuleLoaderBase {
  explicit TextualInterfaceModuleLoader(ASTContext &ctx, StringRef cacheDir,
                                        DependencyTracker *tracker)
    : SerializedModuleLoaderBase(ctx, tracker),
      CacheDir(cacheDir)
  {}

  std::string CacheDir;

  virtual std::error_code
  openModuleFiles(StringRef DirName, StringRef ModuleFilename,
                  StringRef ModuleDocFilename,
                  std::unique_ptr<llvm::MemoryBuffer> *ModuleBuffer,
                  std::unique_ptr<llvm::MemoryBuffer> *ModuleDocBuffer,
                  llvm::SmallVectorImpl<char> &Scratch) override;

public:
  static std::unique_ptr<TextualInterfaceModuleLoader>
  create(ASTContext &ctx, StringRef cacheDir,
         DependencyTracker *tracker = nullptr) {
    return std::unique_ptr<TextualInterfaceModuleLoader>(
        new TextualInterfaceModuleLoader(ctx, cacheDir, tracker));
  }
};


} // end namespace swift

#endif
