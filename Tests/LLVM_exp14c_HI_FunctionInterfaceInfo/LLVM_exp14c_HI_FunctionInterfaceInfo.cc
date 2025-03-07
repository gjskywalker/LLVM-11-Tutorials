#include "LLVM_exp14c_HI_FunctionInterfaceInfo.h"
#include "HI_SysExec.h"
#include "HI_print.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <nlohmann/json.hpp> // Include the JSON for Modern C++ library
#include <filesystem>        // Include filesystem for current_path()

void ReplaceAll(std::string &strSource, const std::string &strOld, const std::string &strNew)
{
    int nPos = 0;
    while ((nPos = strSource.find(strOld, nPos)) != strSource.npos)
    {
        strSource.replace(nPos, strOld.length(), strNew);
        nPos += strNew.length();
    }
}

void pathAdvice()
{
    std::cout << "==============================================================================="
              << std::endl;
    std::cout << "if undefined reference occurs, please check whether the following include paths "
                 "are required."
              << std::endl;
    std::string line;
    std::string cmd_str = "clang++ ../testcase/test.c  -v 2> ciinfor";
    print_cmd(cmd_str.c_str());
    sysexec(cmd_str.c_str());
    std::ifstream infile("ciinfor");
    while (std::getline(infile, line))
    {
        if (line.find("#include <...> search starts here") != std::string::npos)
        {
            while (std::getline(infile, line))
            {
                if (line.find("End of search list.") != std::string::npos)
                {
                    break;
                }
                else
                {
                    ReplaceAll(line, " ", "");
                    ReplaceAll(line, "\n", "");
                    //  CI.getHeaderSearchOpts().AddPath(line,frontend::ExternCSystem,false,true);
                    line = "Potential Path : " + line;
                    print_info(line.c_str());
                }
            }
            break;
        }
    }
    std::cout << "==============================================================================="
              << std::endl;
}

void compile_cmd_generate(const std::string &filename)
{
    nlohmann::json root = nlohmann::json::array();
    nlohmann::json entry;

    // Set the directory where the compilation is happening
    entry["directory"] = std::filesystem::current_path().string();

    // Set the command to compile the file, including additional include paths
    entry["command"] = "clang++ -std=c++17 "
                       "-I/usr/lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12 "
                       "-I/usr/lib/gcc/x86_64-linux-gnu/12/../../../../include/x86_64-linux-gnu/c++/12 "
                       "-I/usr/lib/gcc/x86_64-linux-gnu/12/../../../../include/c++/12/backward "
                       "-I/usr/local/lib/clang/18/include "
                       "-I/usr/local/include "
                       "-I/usr/include/x86_64-linux-gnu "
                       "-I/usr/include " +
                       filename;

    // Set the file being compiled
    entry["file"] = filename;

    // Add the entry to the root array
    root.push_back(entry);

    // Write the JSON to a file
    std::ofstream file("compile_commands.json");
    file << root.dump(4); // Pretty-print with an indent of 4 spaces
    file.close();
}

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;

static llvm::cl::OptionCategory StatSampleCategory("Stat Sample");

int main(int argc, const char **argv)
{
    pathAdvice();

    std::string topfunc = "_2mm";
    if (argc > 1)
    {
        std::string filename = argv[1];
        compile_cmd_generate(filename);
    }
    else
    {
        llvm::errs() << "Error: No input file specified.\n";
        return 1;
    }

    auto ExpectedParser = CommonOptionsParser::create(argc, argv, StatSampleCategory);
    if (!ExpectedParser)
    {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = *ExpectedParser;

    // Create a Clang Tool instance
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
    Rewriter TheRewriter;

    std::map<std::string, int> FuncParamLine2OutermostSize;
    // run the Clang Tool, creating a new FrontendAction, which will run the AST consumer
    Tool.run(HI_FunctionInterfaceInfo_rewrite_newFrontendActionFactory<HI_FunctionInterfaceInfo_FrontendAction>("PLog", TheRewriter,
                                                                                                                "rewriteOut", FuncParamLine2OutermostSize, topfunc, 1)
                 .get());
    // Tool.run(HI_LoopLabeler_rewrite_newFrontendActionFactory<HI_LoopLabeler_FrontendAction>("PLog", TheRewriter,
    //                                                                                         "rewriteOut")
    //              .get());

    return 0;
}

// ANOTHER VERSION OF IMPLEMENTATION
// in which I construct a lot of components according to some other codes
// The following part is actually what the frontendAction does in the function
// BeginSourceFileAction(), initializing and configuring infrastructures.

// using namespace clang;

// class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor>
// {
// public:
//     MyASTVisitor(Rewriter &R)
//         : TheRewriter(R)
//     {}

//     bool VisitStmt(Stmt *s) {

//         return true;
//     }

//     bool VisitFunctionDecl(FunctionDecl *f) {

//         return true;
//     }

// private:
//     void AddBraces(Stmt *s);

//     Rewriter &TheRewriter;
// };

// // Implementation of the ASTConsumer interface for reading an AST produced
// // by the Clang parser.
// class MyASTConsumer : public ASTConsumer
// {
// public:
//     MyASTConsumer(Rewriter &R)
//         : Visitor(R)
//     {}

//     // Override the method that gets called for each parsed top-level
//     // declaration.
//     virtual bool HandleTopLevelDecl(DeclGroupRef DR)
//     {
//         for (DeclGroupRef::iterator b = DR.begin(), e = DR.end(); b != e; ++b)
//         {
//             // Traverse the declaration using our AST visitor.
//             Visitor.TraverseDecl(*b);
//         }
//         return true;
//     }

// private:
//     MyASTVisitor Visitor;
// };

// std::string transform(std::string fileName)
// {

//     std::string cmd_str = "clang++ ../testcase/test.c  -v 2> ciinfor";
//     print_cmd(cmd_str.c_str());
//     bool result = sysexec(cmd_str.c_str());
//     std::ifstream infile("ciinfor");

//     CompilerInstance compilerInstance;
//     compilerInstance.createDiagnostics();

//     CompilerInvocation & invocation = compilerInstance.getInvocation();

//     // Initialize target info with the default triple for our platform.
//     auto TO = std::make_shared<TargetOptions>();
//     TO->Triple = llvm::sys::getDefaultTargetTriple();
//     TargetInfo* targetInfo =
//         TargetInfo::CreateTargetInfo(compilerInstance.getDiagnostics(), TO);
//     compilerInstance.setTarget(targetInfo);

//     compilerInstance.createFileManager();
//     auto& fileManager = compilerInstance.getFileManager();

//     compilerInstance.createSourceManager(fileManager);
//     auto& sourceManager = compilerInstance.getSourceManager();

//     LangOptions langOpts;
//     langOpts.GNUMode = 1;
//     langOpts.CXXExceptions = 1;
//     langOpts.RTTI = 1;
//     langOpts.Bool = 1;   // <-- Note the Bool option here !
//     langOpts.CPlusPlus = 1;

//         std::string line;
//         while (std::getline(infile, line))
//         {
//             if (line.find("#include <...> search starts here")!=std::string::npos)
//             {
//                 while (std::getline(infile, line))
//                 {
//                     if (line.find("End of search list.")!=std::string::npos)
//                     {
//                         break;
//                     }
//                     else
//                     {
//                         ReplaceAll(line," ","");
//                         ReplaceAll(line,"\n","");
//                         compilerInstance.getHeaderSearchOpts().AddPath(line,frontend::ExternCSystem,false,true);
//                     }

//                 }
//                 break;
//             }
//         }

//     PreprocessorOptions &PPOpts = compilerInstance.getPreprocessorOpts();

//     std::cout << TO->Triple<< std::endl;
//     std::cout << (llvm::Triple(TO->Triple)).getOSName().str() << std::endl;
//     invocation.setLangDefaults(langOpts,
//                                 clang::InputKind(),
//                                 llvm::Triple(TO->Triple),
//                                 PPOpts,
//                                 clang::LangStandard::lang_c11);

//     /*

//     static void setLangDefaults(LangOptions &Opts, InputKind IK,
//                     const llvm::Triple &T, PreprocessorOptions &PPOpts,
//                     LangStandard::Kind LangStd = LangStandard::lang_unspecified);

//     */

//     compilerInstance.createPreprocessor(TU_Complete);
//     compilerInstance.createASTContext();

//     // A Rewriter helps us manage the code rewriting task.
//     auto rewriter = clang::Rewriter(sourceManager, compilerInstance.getLangOpts());

//     // Set the main file handled by the source manager to the input file.
//     const FileEntry* inputFile = fileManager.getFile(fileName);
//     sourceManager.setMainFileID(
//         sourceManager.createFileID(inputFile, SourceLocation(), SrcMgr::C_User));
//     compilerInstance.getDiagnosticClient().BeginSourceFile(
//         compilerInstance.getLangOpts(), &compilerInstance.getPreprocessor());

//     // Create an AST consumer instance which is going to get called by
//     // ParseAST.
//     MyASTConsumer consumer(rewriter);

//     // Parse the file to AST, registering our consumer as the AST consumer.
//     clang::ParseAST(
//         compilerInstance.getPreprocessor(),
//         &consumer,
//         compilerInstance.getASTContext());

//     // At this point the rewriter's buffer should be full with the rewritten
//     // file contents.
//     const RewriteBuffer* buffer = rewriter.getRewriteBufferFor(sourceManager.getMainFileID());

//     return std::string(buffer->begin(), buffer->end());
// }