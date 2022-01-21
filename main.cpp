#include <iostream>
#include <cppuhelper/bootstrap.hxx>
#include <rtl/bootstrap.hxx>
#include <beans/XPropertySet.hpp>
#include <bridge/XUnoUrlResolver.hpp>
#include <frame/Desktop.hpp>
#include <frame/XComponentLoader.hpp>
#include <frame/XStorable.hpp>
#include <lang/XMultiComponentFactory.hpp>
#include <text/XTextDocument.hpp>

int main()
{
    //Abs paths should be fixed
    rtl::OUString sEMFUrl("file:///~/Downloads/test.emf"), sJPGUrl("file:///~/Downloads/test.jpg");
    rtl::OUString sConnectionString("uno:socket,host=localhost,port=2083;urp;StarOffice.ServiceManager");
    rtl::Bootstrap::set("URE_MORE_TYPES", "file:///~/Desktop/libreoffice/instdir/program/types/offapi.rdb");
    rtl::Bootstrap::set("URE_BOOTSTRAP", "vnd.sun.star.pathname:/~/Desktop/libreoffice/instdir/program/fundamentalrc");

    auto xComponentContext(cppu::defaultBootstrap_InitialComponentContext());
    auto xMultiComponentFactoryClient(xComponentContext->getServiceManager());
    auto xInterface = xMultiComponentFactoryClient->createInstanceWithContext(
            "com.sun.star.bridge.UnoUrlResolver", xComponentContext);
    auto resolver = css::uno::Reference<css::bridge::XUnoUrlResolver>(xInterface, css::uno::UNO_QUERY);
    try
    {
        xInterface = css::uno::Reference<css::uno::XInterface>(resolver->resolve(sConnectionString)
                                           , css::uno::UNO_QUERY_THROW);
    }
    catch (css::uno::Exception& e)
    {
        std::cout << "Error: cannot establish a connection using '"
             << sConnectionString << "'" << std::endl << e.Message << std::endl;
        exit(1);
    }

    auto xPropSet = css::uno::Reference<css::beans::XPropertySet>(xInterface, css::uno::UNO_QUERY);
    xPropSet->getPropertyValue("DefaultContext") >>= xComponentContext;
    auto xMultiComponentFactoryServer(xComponentContext->getServiceManager());
    auto xComponentLoader = css::frame::Desktop::create(xComponentContext);
    css::uno::Sequence<css::beans::PropertyValue> loadProperties(1);
    loadProperties[0].Name = "Hidden";
    loadProperties[0].Value <<= true;
    try
    {
        //This line fails, but within one of libreoffices internal functions
        auto xComponent = xComponentLoader->loadComponentFromURL(sEMFUrl, "_blank"
                                   , 0, loadProperties);
        auto xDocument = css::uno::Reference<css::text::XTextDocument>(xComponent, css::uno::UNO_QUERY_THROW);
        auto xStorable = css::uno::Reference<css::frame::XStorable>(xDocument, css::uno::UNO_QUERY_THROW);
        auto storeProps = css::uno::Sequence<css::beans::PropertyValue>(2);
        storeProps[0].Name = "FilterName";
        storeProps[0].Value <<= rtl::OUString("writer_jpg_Export");
        storeProps[1].Name = "Overwrite";
        storeProps[1].Value <<= true;
        xStorable->storeToURL(sJPGUrl, storeProps);
    }
    catch(css::uno::Exception& e) {
        std::cout << "Can not open the file ~/Downloads/test.emf" << std::endl;
        return 1;
    }

    css::uno::Reference<css::lang::XComponent>::query(xMultiComponentFactoryClient)->dispose();
    std::cout << "Output ~/Downloads/test.pdf generated." << std::endl;
}
